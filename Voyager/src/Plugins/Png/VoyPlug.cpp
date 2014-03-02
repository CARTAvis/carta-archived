#include "VoyPlug.h"
#include <cmath>
#include <QImage>

VoyPlug::VoyPlug(QObject *parent) :
    QObject(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(VoyagerViewer, VoyPlug)
#endif // QT_VERSION < 0x050000


QString VoyPlug::description()
{
    return "PNG plugin, no math.";
}

double VoyPlug::doMathOnImage(IVoyImage & /*image*/)
{
    return 0;
}

class Png2voyHelper : public IVoyImageReader
{
public:
    Png2voyHelper( QImage img)
    {
        m_qimg = img;
    }

    virtual QString name() const
    {
        return "PNG";
    }

    virtual double operator ()(const std::vector<int> &cursor)
    {
        return qGray( m_qimg.pixel( cursor[0], cursor[1])) / 255.0;
    }
    virtual std::vector<int> dims()
    {
        return std::vector<int> { m_qimg.width(), m_qimg.height() };
    }


    QImage m_qimg;
};

IVoyImageReader *VoyPlug::getImageReader(const QString & fname)
{
    QImage img;
    if( ! img.load( fname, "PNG")) {
        return 0;
    }

    return new Png2voyHelper(img);
}
