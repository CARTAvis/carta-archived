/**
 *
 **/

#include <QDebug>
#include "CCImage.h"

casacore::ImageInterface < casacore::Float > *
cartaII2casaII_float( std::shared_ptr < Carta::Lib::Image::ImageInterface > ii )
{
    // first we convert to base, this seems to work on all platforms
    CCImageBase * base = dynamic_cast<CCImageBase*>( ii.get());
    if( ! base) {
        return nullptr;
    }

    auto latticeBase = base-> getCasaImage();

    // now we try to cast the base to casacore image interface float type
    return dynamic_cast< casacore::ImageInterface < casacore::Float > * > ( latticeBase);

//    casacore::ImageInterface < casacore::Float > * ciif = dynamic_cast< casacore::ImageInterface < casacore::Float > * > ( latticeBase );
//    qWarning() << "ciif" << ciif;
//    return ciif;
}
