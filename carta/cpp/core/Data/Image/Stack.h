/***
 * Represents an indexed group of layers in a stack.
 */

#pragma once
#include "LayerGroup.h"
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
#include "CartaLib/AxisInfo.h"

namespace Carta {

namespace Data {

class DrawStackSynchronizer;
class Selection;

class Stack : public LayerGroup {

friend class Controller;
friend class DrawStackSynchronizer;

Q_OBJECT

public:
    QStringList getCoordinates( double x, double y,
            Carta::Lib::KnownSkyCS system ) const;
    QString getPixelValue( double x, double y) const;


    QString saveImage( const QString& /*saveName*/,  double /*scale*/);
    static const QString CLASS_NAME;
    virtual ~Stack();

protected:

    virtual bool _addGroup( /*const QString& state*/ ) Q_DECL_OVERRIDE;
    virtual bool _closeData( const QString& id ) Q_DECL_OVERRIDE;
    virtual int _getIndexCurrent( ) const;
    /**
     * Return the state of this layer.
     * @return - a string representation of the layer state.
     */
    virtual QString _getStateString() const Q_DECL_OVERRIDE;


    virtual void _resetState( const Carta::State::StateInterface& restoreState ) Q_DECL_OVERRIDE;

    virtual bool _setLayersGrouped( bool grouped  );


    virtual bool _setVisible( const QString& id, bool visible );

private slots:
    void _scheduleFrameReload( bool newClips = false);
    void _viewResize();

private:

    bool _addData(const QString& fileName, std::shared_ptr<ColorState> colorState);

    void _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes, bool applyAll );

    void _gridChanged( const Carta::State::StateInterface& state, bool applyAll);


    /**
     * Return a QImage representation of this data.
     * @param autoClip true if clips should be automatically generated; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void _load( bool autoClip, double clipMinPercentile, double clipMaxPercentile );

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
    int _getIndex( const QString& fileName) const;


    int _getSelectImageIndex() const;

    void _initializeSelections();
    void _initializeState();
    void _render(QList<std::shared_ptr<Layer> > datas, int gridIndex);
    void _renderAll();
    void _renderSingle( int dIndex );
    QString _reorderImages( const std::vector<int> & indices );
    QString _resetFrames( int val);
    void _saveState( bool flush = true );
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

    std::unique_ptr<DrawStackSynchronizer> m_stackDraw;
    Selection* m_selectImage;
    std::vector<Selection*> m_selects;
    bool m_reloadFrameQueued;

    Stack(const Stack& other);
    Stack& operator=(const Stack& other);
};
}
}
