/**
 *
 **/

#include "PixelType.h"
#include "CartaLib.h"
#include <QString>

namespace Carta {
namespace Lib {

int
Image::pixelType2int( const Image::PixelType & type )
{
    static_assert( std::is_same < std::underlying_type < PixelType >::type, int >::value, "NOOO" );
    return static_cast < int > ( type );
}

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


size_t Image::pixelType2size(const Image::PixelType & type){
    switch ( type ){
    case Image::PixelType::Byte :
        return PixelType2CType<Image::PixelType::Byte>::size;
        break;
    case Image::PixelType::Int16 :
        return PixelType2CType<Image::PixelType::Int16>::size;
        break;
    case Image::PixelType::Int32 :
        return PixelType2CType<Image::PixelType::Int32>::size;
        break;
    case Image::PixelType::Int64 :
        return PixelType2CType<Image::PixelType::Int64>::size;
        break;
    case Image::PixelType::Real32 :
        return PixelType2CType<Image::PixelType::Real32>::size;
        break;
    case Image::PixelType::Real64 :
        return PixelType2CType<Image::PixelType::Real64>::size;
        break;
    default :
        CARTA_ASSERT_ALWAYS_X( false, "Bad type conversion");
        break;
    }
    return 0;
}



}
}


