/**
 *
 **/

#pragma once

#include <cstdint>
#include <type_traits>

namespace Image {

enum class PixelType : int {

    Int8 = 0,
    Int16,
    Int32,
    Int64,
    Real32,
    Real64,
    Other

};

static_assert( sizeof(float) == 4, "bad float size");
static_assert( sizeof(double) == 8, "bad double size");

// convenience function convert pixel type to int
int pixelType2int( const PixelType & type) {
    static_assert( std::is_same< std::underlying_type<PixelType>::type, int>::value, "NOOO" );
    return static_cast<int>( type);
}

// convert PixelType to c type
template <PixelType pt>
struct PixelType2CType {};

// specializations
// ================================

// Int8 --> int8_t
template <>
struct PixelType2CType<PixelType::Int8> {
    typedef std::int8_t type;
};

// Real64 --> double
template <>
struct PixelType2CType<PixelType::Real64> {
    typedef double type;
};


// convert ctype to pixeltype
template <typename cType>
struct CType2PixelType {};

template <>
struct CType2PixelType <int8_t> {
    static constexpr PixelType type = PixelType::Int8;
};

template <>
struct CType2PixelType <double> {
    static constexpr PixelType type = PixelType::Real64;
};


}

template <typename SrcType, typename DstType>
struct TypedConverters {
    static const DstType & cvt( const char * ptr) {
        static DstType buffer;
        buffer = static_cast<DstType>(* reinterpret_cast<const SrcType *>( ptr));
        return buffer;
    }
};

template <typename SrcType>
struct TypedConverters < SrcType, SrcType > {
    static const SrcType & cvt( const char * ptr) {
        return * reinterpret_cast<const SrcType *>( ptr);
    }
};

template < typename DstType>
struct Type2CvtFunc{
    typedef const DstType & ( * Type)( const char *);
};

template < typename DstType>
typename Type2CvtFunc<DstType>::Type getConverter( Image::PixelType srcType)
{
    switch (srcType) {
    case Image::PixelType::Int8:
        return & TypedConverters< int8_t, DstType>::cvt;
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
