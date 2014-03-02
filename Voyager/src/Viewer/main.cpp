#include "VoyagerPluginInterface.h"

#include <QPluginLoader>
#include <QCoreApplication>
#include <QDir>
#include <iostream>

#include <casa/Exceptions/Error.h>
#include <images/Images/FITSImage.h>
#include <images/Images/ImageExpr.h>
#include <images/Images/ImageExprParse.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Quanta.h>

struct Globals {
    std::vector< VoyagerPluginInterface * > plugins;
    IVoyImageReader * image;
} globals;


typedef std::vector<int> VI;

class VIIterator
{
public:
    VIIterator( const VI & start, const VI & last) {
        m_start = start;
        m_last = last;
        reset();
    }
    void reset() { m_curr = m_start; }
    bool advance() {
        int pos = m_curr.size() - 1;
        while( pos >= 0) {
            m_curr[pos] ++;
            if( m_curr[pos] <= m_last[pos]) return true;
            m_curr[pos] = m_start[pos];
            pos --;
        }
        return false;
    }
    const VI & curr() const { return m_curr; }
    VI m_start, m_last, m_curr;
};

class NullBuffer : public std::streambuf
{
public:
    int overflow(int c) { return c; }
};

NullBuffer null_buffer;
std::ostream null_stream(&null_buffer);

static std::ostream & dbg( int level = 3)
{
    if( level > 10) return null_stream;
    return std::cout;
}

static void processPlugin( QObject * plugin)
{
    VoyagerPluginInterface * vpint = qobject_cast<VoyagerPluginInterface *>( plugin);
    if( vpint) {
        dbg() << "Loading plugin with description: " << vpint->description().toStdString() << "\n";
        globals.plugins.push_back( vpint);
    }
}

class Casa2VoyImageHelper : public IVoyImageReader
{
public:
    Casa2VoyImageHelper( casa::ImageInterface<float> * casaImage) {
        m_casaImage = casaImage;
    }

    virtual double operator ()(const std::vector<int> &cursor)
    {
        casa::IPosition casaCursor = cursor;
        return m_casaImage-> operator ()( casaCursor);
    }
    virtual std::vector<int> dims()
    {
        std::vector<int> res;
        auto shape = m_casaImage-> shape();
        for( int val : shape) {
            res.push_back( val);
        }
        return res;
    }
    virtual QString name() const
    {
        std::string res = m_casaImage-> imageType();
        return res.c_str();
    }

protected:
    casa::ImageInterface<float> * m_casaImage;
};

static IVoyImageReader * casa2voyimage( casa::ImageInterface<float> * casaImage)
{
    return new Casa2VoyImageHelper( casaImage);
}

static IVoyImageReader * openFile( const QString & fname)
{
    // try to open the image using casacore's fits implementation
    casa::ImageInterface<casa::Float> * img = nullptr;
    try {
        img = new casa::FITSImage( fname.toStdString());
    } catch (...) {
    }

    // if we succeeded, we are done
    if( img) {
        dbg() << "Image opened using casacore\n";
        return casa2voyimage( img);
    }

    // otherwise let's see if we can get one of our plugins to do it
    for( VoyagerPluginInterface * plugin : globals.plugins) {
        dbg() << "Asking plugin '" << plugin-> description().toStdString()
              << "' to open file...\n";
        IVoyImageReader * img = plugin-> getImageReader( fname);
        if( img) {
            dbg() << "Yay, we opened it\n";
            return img;
        }
    }

    // could not open the image with plugins
    return 0;

    //    if( ! img) {
    //        dbg(1) << "Casa failed to open the image...\n";
    //        return nullptr;
    //    }

    //    // just for fun, report the value of the first pixel
    //    int dims = img-> ndim();
    //    dbg() << "img ndim = " << dims << "\n";
    //    std::vector<int> cursor( dims, 0);
    //    double val;
    //    cursor[0] = 0; cursor[1] = 0;
    //    val = img-> getAt( IPosition( cursor));
    //    dbg() << "pixel[" << cursor[0] << "," << cursor[1] << "]= " << val << "\n";
    //    cursor[0] = 1; cursor[1] = 0;
    //    val = img-> getAt( IPosition( cursor));
    //    dbg() << "pixel[" << cursor[0] << "," << cursor[1] << "]= " << val << "\n";
    //    cursor[0] = 0; cursor[1] = 1;
    //    val = img-> getAt( IPosition( cursor));
    //    dbg() << "pixel[" << cursor[0] << "," << cursor[1] << "]= " << val << "\n";

    //    return casa2ii( img);

}

class Voy2casaHelper : public casa::ImageInterface<float>
{

public:
    Voy2casaHelper( IVoyImageReader * reader) {
        m_reader = reader;
    }

    virtual ImageInterface<float> *cloneII() const
    {
        // use copy constructor
        return new Voy2casaHelper( * this);
    }
    virtual String imageType() const
    {
        return m_reader-> name().toStdString();
    }
    virtual void resize(const TiledShape &/*newShape*/)
    {
        throw "cannot resize this image";
    }

    virtual String name(Bool /*stripPath*/) const
    {
        return "whatever/path";
    }

    virtual IPosition shape() const
    {
        IPosition res = m_reader-> dims();
        return res;
    }

    virtual Bool doGetSlice(Array<float> &buffer, const Slicer &section)
    {
//        dbg() << "doGetSlice " << section.start().toString()
//              << " .. " << section.end().toString() << "\n";
        VI start = section.start().asStdVector();
        VI end = section.end().asStdVector();
        int nDim = start.size();
        VI diff;
        for( size_t i = 0 ; i < start.size() ; i ++ ) { diff.push_back( end[i] - start[i]); }
        VI diffPlusOne = diff;
        for( auto & val : diffPlusOne) { val ++; }
//        dbg() << "diffPlusOne = " << IPosition(diffPlusOne).toString() << "\n";
        buffer.resize( diffPlusOne);
        VIIterator dstIt( VI( nDim, 0), diff);
        VIIterator srcIt( section.start().asStdVector(), section.end().asStdVector());
        while(1) {
            auto dstCursor = dstIt.curr();
            auto srcCursor = srcIt.curr();
//            dbg(1) << "src=" << IPosition( srcCursor).toString() << "\n";
//            dbg(1) << "dst=" << IPosition( dstCursor).toString() << "\n";
            buffer(dstCursor) = m_reader-> operator ()(srcCursor);
            dstIt.advance();
            if( ! srcIt.advance()) break;
        }
        return true;
    }

    virtual Bool ok() const
    {
        return true;
    }

    virtual void doPutSlice (const Array<float>& /*buffer*/, const IPosition& /*where*/,
                             const IPosition& /*stride*/)
    {
        throw "We don't support write";
    }

    virtual LatticeRegion * getRegionPtr() const
    {
        return nullptr;
    }

protected:
    IVoyImageReader * m_reader;
};

casa::ImageInterface<float> * voy2casa( IVoyImageReader * reader)
{
    return new Voy2casaHelper(reader);
}

// example function from the documentation that sums something (whole image?)
Float sumPixels(const ImageInterface<Float>& image){
    uInt rowLength = image.shape()(0);
    IPosition rowShape(image.ndim());
    rowShape = 1; rowShape(0) = rowLength;
    Float sumPix = 0;
    RO_LatticeIterator<Float> iter(image, rowShape);
    while(!iter.atEnd()){
//        dbg() << "+\n";
        sumPix += sum(iter.vectorCursor());
        iter++;
    }
    return sumPix;
}

void doCasaOnlyExperiment( const QString & fname)
{
    dbg() << "Trying casa experiment\n";
    casa::FITSImage * img = 0;
    try {
        img = new casa::FITSImage( fname.toStdString());
    } catch (...) {
    }

    if( ! img) {
        dbg() << "Could not open the file using casa alone\n";
        return;
    }

    double val = sumPixels( * img);
    dbg() << "Sum using casacore algorithm = " << val << "\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    VIIterator sit( {1,2,3}, { 2,2,4});
//    VIIterator sit( {0,0,0}, { 80,0,0});
//    while( 1) {
//        auto pos = sit.curr();
//        for( auto x : pos) { dbg() << x << " "; }
//        dbg() << "\n";
//        if( ! sit.advance()) break;
//    }


    // open the image on the command line
    if( argc != 2 ) {
        std::cerr << "Give me filename\n";
        exit(-1);
    }

    doCasaOnlyExperiment( argv[1]);

    // process all static plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances())
        processPlugin(plugin);

    // process all dynamic plugins
    auto pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        dbg() << "Going to load plugin: " << fileName.toStdString() << "\n";
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            dbg() << "Yup, it's a plugin.";
            processPlugin(plugin);
        } else {
            dbg() << "Ooops, it's not a plugin.\n";
        }
    }

    // now load the image with all plugins active
    globals.image = openFile( argv[1]);
    if( ! globals.image) {
        dbg() << "Could not open file " << argv[1] << "\n";
        exit(-1);
    }

    // call doMathOnImage() from every plugin
    for( VoyagerPluginInterface * plugin : globals.plugins ) {
        dbg() << "Doing math using plugin " << plugin->description().toStdString() << "\n";
        auto res = plugin-> doMathOnImage( * globals.image);

        dbg() << "doMath = " << res << "\n";

    }

    // see if we can use pure casacore algorithm on this image
    casa::ImageInterface<float> * casaImage = voy2casa( globals.image);
    double val = sumPixels( * casaImage);
    dbg() << "Sum using casacore algorithm = " << val << "\n";

}
