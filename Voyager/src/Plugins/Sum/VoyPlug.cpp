#include "VoyPlug.h"
#include <cmath>

VoyPlug::VoyPlug(QObject *parent) :
    QObject(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(VoyagerViewer, VoyPlug)
#endif // QT_VERSION < 0x050000


QString VoyPlug::description()
{
    return "Sums pixels of first row.";
}

double VoyPlug::doMathOnImage(IVoyImage &image)
{
    // let's sum up all pixels of the first row
    double sum = 0.0;
    std::vector<int> cursor( image.dims().size(), 0);
    for( int x = 0 ; x < image.dims()[0] ; x ++) {
        cursor[0] = x;
        double val = image(cursor);
        if( std::isfinite(val)) {
            sum += val;
        }
    }
    return sum;
}
