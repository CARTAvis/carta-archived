/**
 * This plugin can read region formats that CASA supports.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include "casacore/casa/Quanta/Quantum.h"
#include "casacore/coordinates/Coordinates/CoordinateSystem.h"
#include "imageanalysis/Annotations/AnnotationBase.h"
#include <QObject>

namespace Carta {
    namespace Lib {
        namespace Regions {
            class Polygon;
            class RegionBase;
        }
    }
}

class RegionCASA : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    /**
     * Constructor.
     */
    RegionCASA(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual ~RegionCASA();

private:

    /**
     * Add the corners to the region information.
     * @param rInfo - the region information.
     * @param corners - the list of region corners to add.
     */
    void _addCorners( Carta::Lib::Regions::Polygon* rInfo,
            const std::vector<QPointF>& corners );

    /**
     * Get a list of the corner points of a region in pixels.
     * @param corners - a list of corner points in world units.
     * @param csys - the coordinate system of the containing image.
     * @param directions - a list of MDirections for the image.
     * @return - a list of corner points of a region in pixels.
     */
    std::vector<QPointF>
        _getPixelVertices( const casa::AnnotationBase::Direction& corners,
            const casa::CoordinateSystem& csys, const casa::Vector<casa::MDirection>& directions ) const;

    /**
     * Convert the length is world coordinates to pixel coordinates.
     * @param centerRadian - an initial point of the radius in world coordinates.
     * @param centerPixel - the corresponding initial point in pixel coordinates.
     * @param radius - a length in world coordinates.
     * @param cSys - the image coordinate system.
     * @return - the corresponding length in pixel coordinates.
     */
    double _getRadiusPixel( const QPointF& centerRadian, const QPointF& centerPixel,
       		double radius, double angleDegrees, const casa::CoordinateSystem& cSys ) const;

    /**
     * Get a lists of x- and y- coordinates of the corner points of a region based on world
     * coordinates.
     * @param x - a list of the x-coordinates of corner points.
     * @param y - a list of the y-coordinates of corner points.
     * @param csys - the coordinate system of the containing image.
     * @param directions - a list of MDirections for the image.
     */
    void _getWorldVertices(std::vector<casa::Quantity>& x, std::vector<casa::Quantity>& y,
            const casa::CoordinateSystem& csys,
            const casa::Vector<casa::MDirection>& directions ) const;

    /**
     * Returns true if the region is a casa region; false otherwise.
     * @param fileName - the absolute path to a file containing the region.
     * @return - true if the file is a recognized region in CASA format; false otherwise.
     */
    bool _isCASARegion( const QString& fileName ) const;

    /**
     * Load one or more regions based on the name of a file specifying regions in CASA format
     * and an image that will contain the region.
     * @param fileName - path to a .crtf file specifying one or more regions in CASA format.
     * @param imagePtr - the image that will contain the region(s).
     * @return - a list containing information about the regions that were loaded.
     */
    std::vector<Carta::Lib::Regions::RegionBase*>
    _loadRegion(const QString & fileName, std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr );

    /**
     * Convert a point in world coordinates to pixel coordinates.
     * @param cSys - the image coordinate system.
     * @param x - the world x-coordinate.
     * @param y - the world y-coordinate.
     * @param successful - set to true if the point is successfully converted; otherwise set to false.
     * @return - a list of the corresponding pixel coordinates.
     */
    casa::Vector<casa::Double> _toPixel( const casa::CoordinateSystem& cSys,
    		double x, double y, bool* successful ) const;
};
