/**
 *
 **/

#pragma once

#include <QString>
#include <cstdint>
#include <type_traits>

namespace Carta {
namespace Lib {
namespace Image {

/// supported pixel types in images
enum class PixelType : int {

    Byte = 0,
    Int16,
    Int32,
    Int64,
    Real32,
    Real64,
    Other

};

static_assert( sizeof(float) == 4, "bad float size");
static_assert( sizeof(double) == 8, "bad double size");

/// convenience function convert pixel type to int
int pixelType2int( const PixelType & type);

/// return size of pixel type in bytes
size_t pixelType2size( const PixelType & type);

/// \todo rename PixelType2CType struct to PixelTypeInfo, add stringName to it,
/// and adjust toStr() to take advantage of it

/// convert PixelType to c type
template <PixelType pt>
struct PixelType2CType {};

// specializations
// ================================

// Byte --> uint8_t
template <>
struct PixelType2CType<PixelType::Byte> {
    typedef std::uint8_t type;
    static constexpr size_t size = 1;
};

// Real64 --> double
template <>
struct PixelType2CType<PixelType::Real64> {
    typedef double type;
    static constexpr size_t size = 8;
};

// Real32 --> float
template <>
struct PixelType2CType<PixelType::Real32> {
    typedef float type;
    static constexpr size_t size = 4;
};

// Int64 --> int64_t
template <>
struct PixelType2CType<PixelType::Int64> {
    typedef int64_t type;
    static constexpr size_t size = 8;
};

// Int32 --> int32_t
template <>
struct PixelType2CType<PixelType::Int32> {
    typedef int32_t type;
    static constexpr size_t size = 4;
};

// Int16 --> int16_t
template <>
struct PixelType2CType<PixelType::Int16> {
    typedef int16_t type;
    static constexpr size_t size = 2;
};

// convert ctype to pixeltype
template <typename cType>
struct CType2PixelType {};

template <>
struct CType2PixelType <std::uint8_t> {
    static constexpr PixelType type = PixelType::Byte;
};

template <>
struct CType2PixelType <double> {
    static constexpr PixelType type = PixelType::Real64;
};

template <>
struct CType2PixelType <float> {
    static constexpr PixelType type = PixelType::Real32;
};

template <>
struct CType2PixelType <std::int32_t> {
    static constexpr PixelType type = PixelType::Int32;
};

template <>
struct CType2PixelType <std::int16_t> {
    static constexpr PixelType type = PixelType::Int16;
};

template <>
struct CType2PixelType <std::int64_t> {
    static constexpr PixelType type = PixelType::Int64;
};

}


/// template ton convert from one type to another
template <typename SrcType, typename DstType>
struct TypedConverters;

/// specialized for identical type
template <typename SrcType>
struct TypedConverters < SrcType, SrcType > {
    static const SrcType & cvt( const char * ptr) {
        return * reinterpret_cast<const SrcType *>( ptr);
    }
};

/// general case
template <typename SrcType, typename DstType>
struct TypedConverters {
    static const DstType & cvt( const char * ptr) {
        static DstType buffer;
        buffer = static_cast<DstType>(* reinterpret_cast<const SrcType *>( ptr));
        return buffer;
    }
};


template < typename DstType>
struct Type2CvtFunc{
    typedef const DstType & ( * Type)( const char *);
};

template < typename DstType>
typename Type2CvtFunc<DstType>::Type getConverter( Carta::Lib::Image::PixelType srcType)
{
    switch (srcType) {
    case Image::PixelType::Byte:
        return & TypedConverters< uint8_t, DstType>::cvt;
        break;
    case Image::PixelType::Int16:
        return & TypedConverters< int16_t, DstType>::cvt;
        break;
    case Image::PixelType::Int32:
        return & TypedConverters< int32_t, DstType>::cvt;
        break;
    case Image::PixelType::Real32:
        return & TypedConverters< float, DstType>::cvt;
        break;
    case Image::PixelType::Real64:
        return & TypedConverters< double, DstType>::cvt;
        break;
    default:
        return nullptr;
        break;
    }
}

/// convenience function to convert a type to a string
QString toStr( Image::PixelType t);


}
}

