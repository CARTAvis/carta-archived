/***
 * Manages a zoom view of an image.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include <QObject>

namespace Carta {

namespace Data {

class Controller;
class LinkableImpl;
class DrawStackSynchronizer;
class Settings;

class ImageZoom : public QObject, public Carta::State::CartaObject,  public ILinkable  {

    Q_OBJECT

public:

    //ILinkable
    virtual QList<QString> getLinks()  const Q_DECL_OVERRIDE;

    /**
     * Adds a link to this animator.
     * @param cartaObject the link that will be managed.
     * @return an error message if there was a problem adding the link; otherwise an empty string.
     */
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Return a representation of the state of the image context.
     * @param sessionId - an identifier for a user session.
     * @param type - the type of state wanted.
     * @return - the image context state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Get the enlargement factor of the zoom window over the main image view
     * window.
     * @return - the enlargement factor.
     */
    int getZoomFactor() const;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Removes a link to this animator.
     * @param cartaObject the link to remove.
     * @return an error message if the link could not be removed; an empty string otherwise.
     */
    virtual QString removeLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Reset the state of the image zoom from a stored value.
     * @param state - a string representation of the state.
     */
    void resetState( const QString& state );

    /**
     * Set the color used to draw the pixel box.
     * @param redValue - the amount of red in the pixel box [0,255].
     * @param greenValue - the amount of green in the pixel box [0,255].
     * @param blueValue - the amount of blue in the pixel box [0,255].
     * @return - an error message if the color could not be set; otherwise, an empty string.
     */
    QString setBoxColor( int redValue, int greenValue, int blueValue);

    /**
     * Set the line width of the pixel box.
     * @param width - the width of the pen used to draw the pixel box.
     * @return - an error message if the line width could not be set; otherwise, an empty string.
     */
    QString setBoxLineWidth( int width );

    /**
     * Set whether or not the pixel box should be shown.
     * @param visible - true to show the pixel box; false, otherwise.
     */
    void setBoxVisible( bool visible);

    /**
     * Set the location of the pixel box.
     * @param br - the bottom right corner of the box.
     * @param tl - the top left corner of the box.
     */
    void setPixelRectangle( QPointF br, QPointF tl );

    /**
     * Store the current tab index.
     * @param tabIndex - the current tab index.
     */
    QString setTabIndex( int index );

    /**
     * Set the enlargement factor of the zoom window over the main image
     * window.
     * @param zoomFactor - the zoom factor.
     */
    QString setZoomFactor( int zoomFactor );

    virtual ~ImageZoom();

    const static QString CLASS_NAME;


private slots:

    void _zoomChanged();

private:

    const static QString BOX;
    const static QString BOX_VISIBLE;
    const static QString CORNER_0;
    const static QString CORNER_1;
    const static QString ZOOM_FACTOR;
    static const int ENLARGE;
    const static int PEN_FACTOR;

    class Factory;
    ImageZoom( const QString& path, const QString& id );

    void _clearDraw();
    void _clearView();

    Controller* _getControllerSelected() const;
    QString _getPreferencesId() const;

    void _initializeCallbacks();
    void _initializeDefaultPen( const QString& key, int red, int green, int blue,
            int alpha, int width );
    void _initializeDefaultState();

    void _refreshView();

    bool _setColor( const QString& key, const QString& majorKey,
            const QString& userId, int colorAmount, QString& errorMsg );

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    std::unique_ptr<Settings> m_settings;

    Carta::State::StateInterface m_stateData;

    std::shared_ptr<DrawStackSynchronizer> m_zoomDraw;

    static bool m_registered;
    ImageZoom( const ImageZoom& other);
    ImageZoom& operator=( const ImageZoom& other );
};
}
}
