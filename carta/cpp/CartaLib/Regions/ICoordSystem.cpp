/**
 *
 **/

#include "ICoordSystem.h"

typedef Carta::Lib::Regions::PointN PointN;

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
    pixel( int ndim = 1)
    {
        return BasicCoordinateSystemInfo( ndim, WcsType::PIXEL, WcsSubType::PIXEL );
    }



private:

    BasicCoordinateSystemInfo( int dims, WcsType type, WcsSubType subType )
    {
        m_ndim = dims;
        m_wcsType = type;
        m_wcsSubType = subType;
    }

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
    CompositeCoordinateSystem( int ndim ) {
        m_cs.resize( ndim, BasicCoordinateSystemInfo::pixel(ndim));
        m_subAxis.resize( ndim, 0);
        for( int i = 0 ; i < ndim ; i ++) {
            m_subAxis[i] = i;
        }
    }

    /// set the type of the axis
    void
    setAxis( int axis, const BasicCoordinateSystemInfo & bcs, int subAxis = 0 )
    {
        CARTA_ASSERT( axis >= 0 && axis < ndim() );
        CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
        m_cs[axis] = bcs;
        m_subAxis[axis] = subAxis;
    }

    /// return the number of axes in this coordinate system
    int
    ndim() const { return m_cs.size(); }

    /// return the coordinate system associated with the selected axis
    const BasicCoordinateSystemInfo &
    cs( int axis ) const
    {
        CARTA_ASSERT( axis >= 0 && axis < ndim() );
        return m_cs[axis];
    }

    /// return the subaxis of the selected axis
    int
    subAxis( int axis ) const
    {
        CARTA_ASSERT( axis >= 0 && axis < ndim() );
        return m_subAxis[axis];
    }

private:

//    std::vector< AxisType> m_axisTypes;
    std::vector < BasicCoordinateSystemInfo > m_cs;
    std::vector < int > m_subAxis;
};


/// the purpose of this class is to format coordinates in an easy to use manner :)
class CoordinateSystemFormatter
{
public:

    /// get the labels for the given coordinate system
    QStringList
    getLabels( const CompositeCoordinateSystem & cs );

    /// return a formatted coordinate for the given coordinate system
    QStringList
    format( const CompositeCoordinateSystem & cs, const PointN & pt );

    /// switch between html and raw text formatting
    void
    setHtml( bool flag )
    {
        m_htmlFlag = flag;
    }

private:

    QString
    getSkyLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatSky( const BasicCoordinateSystemInfo & bcs, int subAxis, double val);

    QString
    getSpectralLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatSpectral( const BasicCoordinateSystemInfo & bcs, int subAxis, double val);

    QString
    getStokesLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatStokes( const BasicCoordinateSystemInfo & bcs, int subAxis, double val);

    QString
    getPixelLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatPixel( const BasicCoordinateSystemInfo & bcs, int subAxis, double val);

    bool m_htmlFlag = false;
};

QStringList
CoordinateSystemFormatter::getLabels( const CompositeCoordinateSystem & cs )
{
    QStringList labels;

    for ( int i = 0 ; i < cs.ndim() ; i++ ) {
        const auto & bcs = cs.cs( i );
        int subAxis = cs.subAxis( i );
        if ( bcs.wcsType() == WcsType::SKY ) {
            labels.append( getSkyLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::SPECTRAL ) {
            labels.append( getSpectralLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::STOKES ) {
            labels.append( getStokesLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::PIXEL ) {
            labels.append( getPixelLabel( bcs, subAxis ) );
        }
        else {
            CARTA_ASSERT( false );
            labels.append( "?" );
        }
    }
    return labels;
} // getLabels

QStringList
CoordinateSystemFormatter::format( const CompositeCoordinateSystem & cs, const PointN & pt )
{
    auto ppt = pt;
    if ( int( ppt.size()) < cs.ndim() ) {
        ppt.resize( cs.ndim(), 0.0 );
    }
    QStringList result;
    for ( int i = 0 ; i < cs.ndim() ; i++ ) {
        const auto & bcs = cs.cs( i );
        int subAxis = cs.subAxis( i );
        if ( bcs.wcsType() == WcsType::SKY ) {
            result.append( formatSky( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::SPECTRAL ) {
            result.append( formatSpectral( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::STOKES ) {
            result.append( formatStokes( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::PIXEL ) {
            result.append( formatPixel( bcs, subAxis, ppt[i] ) );
        }
        else {
            result.append( QString::number( ppt[i]));
        }
    }
    return result;
} // getLabels

QString
CoordinateSystemFormatter::getSkyLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SKY );
    CARTA_ASSERT( subAxis == 0 || subAxis == 1 );
    if ( bcs.wcsSubType() == WcsSubType::GALACTIC ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "l" : "l";
        }
        else {
            return m_htmlFlag ? "l" : "b";
        }
    }
    else if ( bcs.wcsSubType() == WcsSubType::ECLIPTIC ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "&lambda;" : "l";
        }
        else {
            return m_htmlFlag ? "&beta;" : "b";
        }
    }
    else if ( bcs.wcsSubType() == WcsSubType::J2000 ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "&alpha;" : "ra";
        }
        else {
            return m_htmlFlag ? "&delta;" : "dec";
        }
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?sky?";
}

QString CoordinateSystemFormatter::formatSky(const BasicCoordinateSystemInfo & bcs, int subAxis, double val)
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SKY );
    CARTA_ASSERT( subAxis == 0 || subAxis == 1 );
    return QString( "(sky:%1)").arg( val);
}

QString
CoordinateSystemFormatter::getSpectralLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SPECTRAL );
    CARTA_ASSERT( subAxis == 0 );
    if ( bcs.wcsSubType() == WcsSubType::FREQUENCY ) {
        return m_htmlFlag ? "Freq" : "Freq";
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?spectral?";
}

QString CoordinateSystemFormatter::formatSpectral(const BasicCoordinateSystemInfo & bcs, int subAxis, double val)
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SPECTRAL );
    CARTA_ASSERT( subAxis == 0 );
    return QString( "(spec:%1)").arg( val);
}

QString
CoordinateSystemFormatter::getStokesLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::STOKES );
    CARTA_ASSERT( subAxis == 0 );
    if ( bcs.wcsSubType() == WcsSubType::STOKES ) {
        return m_htmlFlag ? "Stokes" : "Stokes";
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?stokes?";
}

QString CoordinateSystemFormatter::formatStokes(const BasicCoordinateSystemInfo &bcs, int subAxis, double val)
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::STOKES );
    CARTA_ASSERT( subAxis == 0 );
    static const QStringList symbs({"I","Q","U","V"});
    int ival = val;
    if( ival >= 0 && ival < symbs.size()) {
        return symbs[ival];
    } else {
        return QString("(%stokes:%1").arg(ival);
    }
}

QString
CoordinateSystemFormatter::getPixelLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::PIXEL );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    if ( bcs.wcsSubType() == WcsSubType::PIXEL ) {
        if ( subAxis < 3 ) {
            return QString( 'x' + subAxis );
        }
        else {
            return QString( "axis%1" ).arg( subAxis );
        }
    }
    else {
        CARTA_ASSERT( false );
    }
    return QString( "(%1)" ).arg( subAxis );
}

QString CoordinateSystemFormatter::formatPixel(const BasicCoordinateSystemInfo &bcs, int subAxis, double val)
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::PIXEL );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    return QString( "(pix:%1)").arg(val);
}


static int
apiTestFormatting()
{
    qDebug() << "Test1: pixel coordinate system";
    {
        CompositeCoordinateSystem cs( 5 );
        CoordinateSystemFormatter f;

        // get the labels for the axes
        qDebug() << "Labels" << f.getLabels( cs );

        // format a coordinate
        qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                          );

    }
    qDebug() << "Test2: spectral cube system";
    // make 4 axis coordinate system
    CompositeCoordinateSystem cs( 4 );

    // first axis will be galactic longitude
    cs.setAxis( 0, BasicCoordinateSystemInfo::galactic(), 0 );

//    cs.setAxis( 0, AxisType::DIRECTION_LON, new GalacticCoordinateSystem );

    // second axis will be galactic lattitude
    cs.setAxis( 1, BasicCoordinateSystemInfo::galactic(), 1 );

//    cs.setAxis( 1, AxisType::DIRECTION_LAT, new GalacticCoordinateSystem );

    // third axis will be frequency
//    cs.setAxis( 2, AxisType::SPECTRAL, new FrequencyCoordinateSystem );
    cs.setAxis( 2, BasicCoordinateSystemInfo::frequency() );

    // last axis will be stokes
//    cs.setAxis( 3, AxisType::STOKES, new StokesCoordinateSystem );
    cs.setAxis( 3, BasicCoordinateSystemInfo::stokes() );

    // make a formatter
    CoordinateSystemFormatter f;

    // get the labels for the axes
    qDebug() << "Labels" << f.getLabels( cs );

    // format a coordinate
    qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                      );

    return 0;

} // apiTestFormatting

static int x = apiTestFormatting();
