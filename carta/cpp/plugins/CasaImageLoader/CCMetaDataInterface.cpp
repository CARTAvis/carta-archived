/**
 *
 **/

#include "CCMetaDataInterface.h"
#include "CCCoordinateFormatter.h"

#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>

CCMetaDataInterface::CCMetaDataInterface( QString htmlTitle,
                                          std::shared_ptr < casa::CoordinateSystem > casaCS )
{
    m_title = Carta::Lib::HtmlString::fromHtml( htmlTitle );
    m_casaCS = casaCS;
}

Carta::Lib::Image::MetaDataInterface *
CCMetaDataInterface::clone()
{
    qFatal( "not implemented" );
}

CoordinateFormatterInterface::SharedPtr
CCMetaDataInterface::coordinateFormatter()
{
    return std::make_shared < CCCoordinateFormatter > ( m_casaCS );
}

std::shared_ptr<casa::CoordinateSystem> CCMetaDataInterface::getCoordinateSystem() const {
    return m_casaCS;
}


PlotLabelGeneratorInterface::SharedPtr
CCMetaDataInterface::plotLabelGenerator()
{
    qFatal( "not implemented" );
}

std::pair<double,QString> CCMetaDataInterface::getRestFrequency() const {
	std::pair<double,QString> restFreq;
	if ( m_casaCS ){
		//Fill in the image rest frequency & unit
		if ( m_casaCS->hasSpectralAxis() ){
			restFreq.first = m_casaCS->spectralCoordinate().restFrequency();
			restFreq.second = m_casaCS->spectralCoordinate().worldAxisUnits()[0].c_str();

		}
	}
	return restFreq;
}

QString
CCMetaDataInterface::title( TextFormat format )
{
    if ( format == TextFormat::Plain ) {
        return m_title.plain();
    }
    else {
        return m_title.html();
    }
}

QStringList
CCMetaDataInterface::otherInfo( TextFormat format )
{
    Q_UNUSED( format );
    qFatal( "not implemented" );
}

class CCCoordSystemConverter : public Carta::Lib::Regions::ICoordSystemConverter
{
public:

    CCCoordSystemConverter( std::shared_ptr < casa::CoordinateSystem > casaCS )
    {
        m_casaCS = casaCS;

        // source coordinate system will be a pixel CS
        m_srcCS = Carta::Lib::Regions::CompositeCoordinateSystem( m_casaCS-> nPixelAxes() );

        // destination CS will be a world CS
        // start by creating a default system with the appropriate number of axes
        m_dstCS = Carta::Lib::Regions::CompositeCoordinateSystem( m_casaCS-> nWorldAxes() );

        qDebug() << "npix=" << m_casaCS-> nPixelAxes() << " nworld=" << m_casaCS-> nWorldAxes();
        qDebug() << "ncoord=" << m_casaCS-> nCoordinates();

        auto wan = m_casaCS-> worldAxisNames().tovector();
        QStringList wanl;
        for ( auto & n : wan ) {
            wanl.append( n.c_str() );
        }
        qDebug() << "wan=" << wanl;
        uint nw = m_casaCS-> nWorldAxes();

        for ( uint i = 0 ; i < nw ; i++ ) {
            int coord, coordAxis;
            m_casaCS-> findWorldAxis( coord, coordAxis, i );
            qDebug() << "  find=" << coord << coordAxis;
            auto & subcs = m_casaCS-> coordinate( coord );
            qDebug() << "    -type:" << subcs.type();

            // warn about subaxis not being 0 for everything but direction
            if( subcs.type() != casa::Coordinate::Type::DIRECTION && coordAxis != 0) {
                qWarning() << "CasaImageLoader plugin: coordAxis should be 0 for non-direction";
                coordAxis = 0;
            }
            // warn about subaxis not being 0 or 1 for direction
            if( subcs.type() == casa::Coordinate::Type::DIRECTION &&
                (coordAxis != 0 && coordAxis != 1))
            {
                qWarning() << "CasaImageLoader plugin: coordAxis should be 0 or 1 for direction";
                coordAxis = 0;
            }

            if ( subcs.type() == casa::Coordinate::Type::DIRECTION ) {
                const casa::DirectionCoordinate & dirc = m_casaCS-> directionCoordinate();
                if ( dirc.directionType() == casa::MDirection::Types::J2000 ) {
                    m_dstCS.setAxis( i,
                                     Carta::Lib::Regions::BasicCoordinateSystemInfo::j2000(),
                                     coordAxis );
                }
                else if ( dirc.directionType() == casa::MDirection::Types::GALACTIC ) {
                    m_dstCS.setAxis( i,
                                     Carta::Lib::Regions::BasicCoordinateSystemInfo::galactic(),
                                     coordAxis );
                }
                else if ( dirc.directionType() == casa::MDirection::Types::ECLIPTIC ) {
                    m_dstCS.setAxis( i,
                                     Carta::Lib::Regions::BasicCoordinateSystemInfo::ecliptic(),
                                     coordAxis );
                }
                else {
                    qWarning() << "CasaImageLoader plugin:"
                               << "Don't know how to convert casa direction"
                               << casa::MDirection::showType( dirc.directionType() ).c_str();
                }
            }
            else if ( subcs.type() == casa::Coordinate::Type::SPECTRAL ) {
                const casa::SpectralCoordinate & specc = m_casaCS-> spectralCoordinate();
                if( specc.nativeType() == casa::SpectralCoordinate::SpecType::FREQ) {
                    m_dstCS.setAxis( i,
                                     Carta::Lib::Regions::BasicCoordinateSystemInfo::frequency() );

                } else {
                    casa::String cstring = "???";
                    casa::SpectralCoordinate::specTypetoString( cstring, specc.nativeType());
                    qWarning() << "CasaImageLoader plugin: "
                               << "Don't know how to convert casa spectral"
                               << cstring.c_str();
                }
            }
            else if ( subcs.type() == casa::Coordinate::Type::STOKES ) {
                const casa::StokesCoordinate & stokesc = m_casaCS-> stokesCoordinate();
                auto s = stokesc.stokes().tovector();
                qDebug() << "stokesv=" << s;
                m_dstCS.setAxis( i,
                                 Carta::Lib::Regions::BasicCoordinateSystemInfo::stokes() );

            }
            else {
                qWarning() << "CasaImageLoader plugin: "
                           << "Don't know how to convert casa coordinate"
                           << m_casaCS-> showType(i).c_str();
            }
        }
    }

    virtual bool
    src2dst( const Carta::Lib::Regions::PointN & pt,
             Carta::Lib::Regions::PointN & result ) override
    {
        /// \todo implement
        Q_UNUSED( pt );
        Q_UNUSED( result );
        return false;
    }

    virtual bool
    dst2src( const Carta::Lib::Regions::PointN & pt,
             Carta::Lib::Regions::PointN & result ) override
    {
        /// \todo implement
        Q_UNUSED( pt );
        Q_UNUSED( result );
        return false;
    }

    virtual const Carta::Lib::Regions::CompositeCoordinateSystem &
    srcCS() override
    {
        return m_srcCS;
    }

    virtual const Carta::Lib::Regions::CompositeCoordinateSystem &
    dstCS() override
    {
        return m_dstCS;
    }

private:

    std::shared_ptr < casa::CoordinateSystem > m_casaCS;
    Carta::Lib::Regions::CompositeCoordinateSystem m_srcCS, m_dstCS;
};

Carta::Lib::Regions::ICoordSystemConverter::SharedPtr
CCMetaDataInterface::getCSConv()
{
    /// \todo cache this, insteady of creating a new one every time
    return std::make_shared < CCCoordSystemConverter > ( m_casaCS );
}
