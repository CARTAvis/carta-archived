/***
 * Represents an indexed group of layers in a stack.
 */

#pragma once
#include "LayerGroup.h"
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/RegionInfo.h"

namespace Carta {


namespace Data {

class DrawStackSynchronizer;
class Region;
class Selection;
class SaveService;

class Stack : public LayerGroup {

friend class Controller;
friend class DrawStackSynchronizer;

Q_OBJECT

public:
    QStringList getCoordinates( double x, double y,
            Carta::Lib::KnownSkyCS system ) const;
    QString getPixelValue( double x, double y) const;

    static const QString CLASS_NAME;
    virtual ~Stack();


signals:

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

protected:

    virtual bool _addGroup( /*const QString& state*/ ) Q_DECL_OVERRIDE;
    virtual bool _closeData( const QString& id ) Q_DECL_OVERRIDE;
    virtual int _getIndexCurrent( ) const;

    virtual void _resetState( const Carta::State::StateInterface& restoreState ) Q_DECL_OVERRIDE;

    virtual bool _setLayersGrouped( bool grouped  );

    virtual bool _setSelected( const QStringList& names ) Q_DECL_OVERRIDE;


    virtual bool _setVisible( const QString& id, bool visible );

private slots:

    void _scheduleFrameReload( bool renderAll = true);

    void _viewResize();

    // Asynchronous result from saveFullImage().
    void _saveImageResultCB( bool result );

private:

    bool _addData(const QString& fileName, std::shared_ptr<ColorState> colorState );
    QString _addDataRegion(const QString& fileName );

    QString _closeRegion( const QString& regionId );

    void _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes, bool applyAll );

    std::vector<Carta::Lib::RegionInfo> _getRegions() const;
    /**
     * Return the state of this layer.
     * @return - a string representation of the layer state.
     */
    QString _getStateString() const;

    void _gridChanged( const Carta::State::StateInterface& state, bool applyAll);


    /**
     * Return a QImage representation of this data.
     * @param renderAll - true if all images in the stack should be drawn; false, for
     *      just the current one.
     * @param autoClip true if clips should be automatically generated; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void _load( bool renderAll, bool autoClip, double clipMinPercentile, double clipMaxPercentile );

    /**
     * Center the image.
     */
    void _resetPan( bool panZoomAll );

    /**
     * Reset the zoom to the original value.
     */
    void _resetZoom( bool panZoomAll );

    std::set<Carta::Lib::AxisInfo::KnownType> _getAxesHidden() const;

    QString _getCursorText( int mouseX, int mouseY );
    int _getFrame( Carta::Lib::AxisInfo::KnownType axisType ) const;
    int _getFrameUpperBound( Carta::Lib::AxisInfo::KnownType axisType ) const;
    std::vector<int> _getFrameIndices( ) const;

    std::vector<int> _getImageSlice() const;
    int _getIndex( const QString& layerId) const;


    int _getSelectImageIndex() const;
    QString _getCurrentId() const;

    void _initializeSelections();
    void _initializeState();
    void _render(QList<std::shared_ptr<Layer> > datas, int gridIndex);
    void _renderAll();
    void _renderSingle( int dIndex );
    QString _reorderImages( const std::vector<int> & indices );
    QString _resetFrames( int val);
    void _saveChildren( Carta::State::StateInterface& state, bool truncate ) const;

    /**
     * Save a copy of the full image in the current image view.
     * @param saveName the full path where the file is to be saved.
     * @return an error message if there was a problem saving the image.
     */
    QString _saveImage( const QString& saveName );


    void _saveState( bool flush = true );
    void _saveStateRegions();
    bool _setCompositionMode( const QString& id, const QString& compositionMode,
               QString& errorMsg );
    void _setFrameAxis(int value, Carta::Lib::AxisInfo::KnownType axisType);
    QString _setFrameImage( int val );
    void _setMaskAlpha( const QString& id, int alphaAmount, QString& result );

    void _setMaskColor( const QString& id, int redAmount,
                       int greenAmount, int blueAmount, QStringList& result );
    void _setPan( double imgX, double imgY, bool all );
    void _setViewName( const QString& viewName );
    void _setZoomLevel( double zoomFactor, bool zoomPanAll );


    void _updatePan( double centerX , double centerY, bool zoomPanAll );
    void _updatePan( double centerX , double centerY,
            std::shared_ptr<Layer> data);

    void _updateZoom( double centerX, double centerY, double zoomFactor, bool zoomPanAll);
    void _updateZoom( double centerX, double centerY, double zoomFactor,
            std::shared_ptr<Layer> data );


    /**
     *  Constructor.
     */
    Stack( const QString& path, const QString& id );

    class Factory;
    static bool m_registered;
    static const QString VIEW;
    static const QString REGIONS;

    std::unique_ptr<DrawStackSynchronizer> m_stackDraw;
    Selection* m_selectImage;
    std::vector<Selection*> m_selects;
    QList<std::shared_ptr<Region> > m_regions;
    bool m_reloadFrameQueued;

    /// Saves images
    SaveService *m_saveService;

    Stack(const Stack& other);
    Stack& operator=(const Stack& other);
};
}
}
