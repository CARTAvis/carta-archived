/**
 *
 **/

#include "CartaLib/CartaLib.h"
#include <QString>

#pragma once

namespace Carta
{
namespace Lib
{
namespace Regions
{
typedef std::vector < double > PointN;

/// constants for known world coordinate types
/// if you add items here, you should create appropriate named constructors
enum class WcsSubType
{
    // SKY systems
    GALACTIC,
    ECLIPTIC,
    J2000,

    // SPECTRAL systems
    FREQUENCY,

    // STOKES systems
    STOKES,

    // pixel systems
    PIXEL
};

/// the list in WcsSubType is pretty large, so the constants below should simplify
/// handling these a bit more
enum class WcsType
{
    SKY,
    SPECTRAL,
    STOKES,
    PIXEL
};

/// the purpose of this class is to hold basic information about a basic world coordinate
/// system, e.g. GALACTIC sky, or FREQUENCY...
///
/// I really did not feel like it was going to be beneficial to design this as
/// some polymorphic set of classes, so I attempted to group all of them into a single
/// class. The immediate benefits are: easily pass by value, easily check for types
/// Negative consequences: possibly harder to extend.
///
class BasicCoordinateSystemInfo
{
public:

    int
    ndim() const { return m_ndim; }

    WcsSubType
    wcsSubType() const { return m_wcsSubType; }

    WcsType
    wcsType() const { return m_wcsType; }

    /// named constructor - for creating a galactic coordinate system
    static BasicCoordinateSystemInfo
    galactic()
    {
        return BasicCoordinateSystemInfo( 2, WcsType::SKY, WcsSubType::GALACTIC );
    }

    /// named constructor - for creating an ecliptic coordinate system
    static BasicCoordinateSystemInfo
    ecliptic()
    {
        return BasicCoordinateSystemInfo( 2, WcsType::SKY, WcsSubType::ECLIPTIC );
    }

    /// named constructor - for creating a J2000 coordinate system
    static BasicCoordinateSystemInfo
    j2000()
    {
        return BasicCoordinateSystemInfo( 2, WcsType::SKY, WcsSubType::J2000 );
    }

    /// named constructor - for creating a frequency coordinate system
    static BasicCoordinateSystemInfo
    frequency()
    {
        return BasicCoordinateSystemInfo( 1, WcsType::SPECTRAL, WcsSubType::FREQUENCY );
    }

    /// named constructor - for creating a stokes coordinate system
    static BasicCoordinateSystemInfo
    stokes()
    {
        return BasicCoordinateSystemInfo( 1, WcsType::STOKES, WcsSubType::STOKES );
    }

    /// named constructor - for creating a pixel coordinate system
    static BasicCoordinateSystemInfo
    pixel( int ndim = 1 )
    {
        return BasicCoordinateSystemInfo( ndim, WcsType::PIXEL, WcsSubType::PIXEL );
    }

private:

    BasicCoordinateSystemInfo( int dims, WcsType type, WcsSubType subType );

    int m_ndim = 1;
    WcsSubType m_wcsSubType;
    WcsType m_wcsType;
};

/// the purpose of this class is to pull together a coordinate system made up of
/// multiple different types of axes (think of spectral cube)
class CompositeCoordinateSystem
{
public:

    /// construct a default coordinate system with ndim axes
    /// the system will be initialized to be all pixels
    CompositeCoordinateSystem( int ndim = 1 );

    /// set the type of the axis
    void
    setAxis( int axis, const BasicCoordinateSystemInfo & bcs, int subAxis = 0 );

    /// return the number of axes in this coordinate system
    int
    ndim() const;

    /// return the coordinate system associated with the selected axis
    const BasicCoordinateSystemInfo &
    cs( int axis ) const;

    /// return the subaxis of the selected axis
    int
    subAxis( int axis ) const;

private:

//    std::vector< AxisType> m_axisTypes;
    std::vector < BasicCoordinateSystemInfo > m_cs;
    std::vector < int > m_subAxis;
};

/// coordinate system converter interface
class ICoordSystemConverter
{
    CLASS_BOILERPLATE( ICoordSystemConverter );

public:

    /// convert point pt given in source coordinate system to a point in destination CS
    /// \param pt input point
    /// \param result resulting point
    /// \return false if conversion was unsuccessful
    virtual bool
    src2dst( const PointN & pt, PointN & result ) = 0;

    /// convert point pt given in destination coordinate system to a point in source CS
    /// (i.e. the reverse of src2dst)
    /// \param pt input point
    /// \param result resulting point
    /// \return false if conversion was unsuccessful
    virtual bool
    dst2src( const PointN & pt, PointN & result ) = 0;

    /// return a map of axes in src to dst
    /// for example, a standard RA,DEC,FREQ,STOKES polarization cube would return
    /// [ 0, 1, 2, 3]
    /// [ 0, 1, 2, 3]
    //    const std::vector< std::vector< int > > & src2dstMap() = 0;
    //    const std::vector< std::vector< int > > & dst2srcMap() = 0;

    virtual const CompositeCoordinateSystem &
    srcCS() = 0;

    virtual const CompositeCoordinateSystem &
    dstCS() = 0;
};

/// implements an identity coordinate system converter for the given composite
/// coordinate system, i.e. both source and destination will be set to the supplied
/// coordinate system, and the transformations will be identities
class IdentityCoordSystemConverter : public ICoordSystemConverter
{
    // ICoordSystemConverter interface

public:

    IdentityCoordSystemConverter( const CompositeCoordinateSystem & cs )
    {
        m_srcCS = cs;
        m_dstCS = cs;
    }

    virtual bool
    src2dst( const PointN & pt, PointN & result ) override
    {
        result = pt;
        return true;
    }

    virtual bool
    dst2src( const PointN & pt, PointN & result ) override
    {
        result = pt;
        return true;
    }

    virtual const CompositeCoordinateSystem &
    srcCS() override
    {
        return m_srcCS;
    }

    virtual const CompositeCoordinateSystem &
    dstCS() override
    {
        return m_dstCS;
    }

private:

    CompositeCoordinateSystem m_srcCS, m_dstCS;
};

/// a pixel identity coordinate system creator
Carta::Lib::Regions::ICoordSystemConverter::UniquePtr makePixelIdentityConverter( int ndim);

/// or you can use this class :)
class PixelIdentityCSConverter : public IdentityCoordSystemConverter {
public:
    PixelIdentityCSConverter( int ndim)
        : IdentityCoordSystemConverter( CompositeCoordinateSystem(ndim))
    {

    }
};

}
}
}


#ifdef DONT_COMPILE

/// description of an axis in an image
class IAxisInfo
{
public:

    /// axis types we recognize
    enum class KnownType
    {
        DIRECTION_LON, /// < direction, longitude-ish
        DIRECTION_LAT, /// < direction, latitude-ish
        SPECTRAL,      /// < spectral axis
        STOKES,        /// < stokes axis
        PIXEL,         /// < pixel axis with no other information
        OTHER
    };

    /// return the type of this axis
    virtual KnownType
    knownType() const = 0;
};


class PixelAxisInfo : public IAxisInfo
{
public:

    virtual KnownType
    knownType() const override
    {
        return KnownType::PIXEL;
    }
};


class OtherAxisInfo : public IAxisInfo
{
public:

    OtherAxisInfo( QString otherInfo )
    {
        m_otherInfo = otherInfo;
    }

    virtual KnownType
    knownType() const override
    {
        return KnownType::OTHER;
    }

    QString
    otherInfo() const
    {
        return m_otherInfo;
    }

private:

    QString m_otherInfo;
};


class ICoordSystem
{
public:

    /// return a unique string (compact JSON)
    virtual QString
    serialized() = 0;

    /// compare to another coordinate system
    virtual bool
    isSameAs( ICoordSystem * otherCS ) = 0;

    /// return the world coordinate system (could be * this)
//    const ICoordSystem & worldCS();

    /// return the

    /// return information about axes
    virtual std::vector < const IAxisInfo * > &
    axisInfo() = 0;
};

/// default coordinate system - i.e. none
class DefaultCoordSystem : public ICoordSystem
{
public:

    DefaultCoordSystem( int nCoords = 2 )
    {
        CARTA_ASSERT( nCoords > 0 );
        m_axisInfos.resize( nCoords );
        for ( auto & ai : m_axisInfos ) {
            ai = new PixelAxisInfo();
        }
    }

    virtual QString
    serialized() override { return ""; }

    virtual bool
    isSameAs( ICoordSystem * otherCS ) override
    {
        return serialized() == otherCS->serialized();
    }

    virtual std::vector < IAxisInfo const * > &
    axisInfo() override
    {
        return m_axisInfos;
    }

private:

    std::vector < const IAxisInfo * > m_axisInfos;
};


/// implements the simplest coordinate system converter - an identity
/// both source and destination coordinate systems will be pixel
/// conversion from one to the other will result in no transformation (identity)
class DefaultCoordSystemConverter : public ICoordSystemConverter
{
public:

    DefaultCoordSystemConverter( int ndim );

    virtual bool
    src2dst( const PointN & src, PointN & dst ) override;

    virtual bool
    dst2src( const PointN & src, PointN & dst ) override;

    virtual const CompositeCoordinateSystem &
    srcCS() override;

    virtual const CompositeCoordinateSystem &
    dstCS() override;

private:

    CompositeCoordinateSystem m_srcCS, m_dstCS;
};
#endif
