/**
 *
 **/

#include "PixelType.h"
#include <QString>

int
Image::pixelType2int( const Image::PixelType & type )
{
    static_assert( std::is_same < std::underlying_type < PixelType >::type, int >::value, "NOOO" );
    return static_cast < int > ( type );
}

//QString Image::pixelType2String(Image::PixelType t)
//{
//    switch (t) {
//    case PixelType::Byte:
//        return "byte";
//        break;
//    case PixelType::Byte:
//        return "byte";
//        break;
//    default:
//        return QString("unknown(%1 bytes)").arg( sizeof( pixelType2int()));
//        break;
//    }
//}

namespace Carta {


QString
toStr( Image::PixelType t )
{
    switch ( t )
    {
    case Image::PixelType::Byte :
        return "byte";

        break;
    case Image::PixelType::Int16 :
        return "int16";

        break;
    case Image::PixelType::Int32 :
        return "int32";

        break;
    case Image::PixelType::Int64 :
        return "int64";

        break;
    case Image::PixelType::Real32 :
        return "real32";

        break;
    case Image::PixelType::Real64 :
        return "real64";

        break;
    default :
        return "???";

        break;
    }
}

}
