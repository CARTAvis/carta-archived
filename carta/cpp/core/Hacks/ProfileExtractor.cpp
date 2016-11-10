/**
 *
 **/

#include "ProfileExtractor.h"

namespace Carta
{
namespace Lib
{
namespace Profiles
{
IProfileExtractor *
getBestProfileExtractor( Carta::Lib::NdArray::RawViewInterface * rv,
                         ProfilePathType pt )
{
    Q_UNUSED( rv );
    Q_UNUSED( pt );
    return new DefaultPrincipalProfileExtractor;
}

const std::vector < double >
ProfileExtractor::getDataD()
{
    // allocate memory for the result
    qint64 avail = getRawDataLength();
    if ( avail <= 0 ) {
        return { };
    }
    std::vector < double > result( avail );

    // copy the available pixels
    const char * src = m_resultBuffer.constData();
    double * dst = & result[0];
    const double & ( * cvt )(const char *);
    cvt = Carta::Lib::getConverter < double > ( m_rawView->pixelType() );
    for ( qint64 i = 0 ; i < avail ; i++ ) {
        * dst = cvt( src );

        src += m_pixelSize;
        dst += 1;
    }

    return result;
} // getDataD
}
}
}
