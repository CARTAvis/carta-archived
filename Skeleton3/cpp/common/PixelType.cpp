/**
 *
 **/

#include "PixelType.h"

int Image::pixelType2int(const Image::PixelType & type) {
    static_assert( std::is_same< std::underlying_type<PixelType>::type, int>::value, "NOOO" );
    return static_cast<int>( type);
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
