/***
 * Provides the context view of an image.
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

class ImageContext : public QObject, public Carta::State::CartaObject,  public ILinkable  {

    Q_OBJECT

public:

    //Linkable interface.
    virtual QList<QString> getLinks()  const Q_DECL_OVERRIDE;

    /**
     * Adds a link to this animator.
     * @param cartaObject the link that will be managed.
     * @return an error message if there was a problem adding the link; otherwise an empty string.
     */
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Return the image context rectangle in pixel coordinates.
     * @return - the image context rectangle in pixel coordinates.
     */
    QRectF getImageRectangle() const;

    /**
     * Return a representation of the state of the image context.
     * @param sessionId - an identifier for a user session.
     * @param type - the type of state wanted.
     * @return - the image context state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

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
     * Reset the state.
     * @param state - a representation of the state to restore.
     */
    void resetState( const QString& state );

    /**
     * Set the color of the image box.
     * @param redValue - the amount of red in the box.
     * @param greenValue - the amount of green in the box.
     * @param blueValue - the amount of blue in the box.
     * @return - an error message if there was a problem setting the color; an empty string
     *      otherwise.
     */
    QString setBoxColor( int redValue, int greenValue, int blueValue);

    /**
     * Set the color of the xy-compass.
     * @param redValue - the amount of red in the compass.
     * @param greenValue - the amount of green in the compass.
     * @param blueValue - the amount of blue in the compass.
     * @return - an error message if there was a problem setting the color; an empty string
     *      otherwise.
     */
    QString setCompassXYColor( int redValue, int greenValue, int blueValue);

    /**
     * Set the color of the ne-compass.
     * @param redValue - the amount of red in the compass.
     * @param greenValue - the amount of green in the compass.
     * @param blueValue - the amount of blue in the compass.
     * @return - an error message if there was a problem setting the color; an empty string
     *      otherwise.
     */
    QString setCompassNEColor( int redValue, int greenValue, int blueValue);

    /**
     * Set the pen thickness for the image box.
     * @param width - the pen thickness used to draw the image box.
     * @return - an error message if the pen thickness could not be set; an empty
     *      string otherwise.
     */
    QString setBoxLineWidth( int width );

    /**
     * Set the pen thickness for the xy-compass.
     * @param width - the pen thickness used to draw the xy-compass.
     * @return - an error message if the pen thickness could not be set; an empty
     *      string otherwise.
     */
    QString setCompassXYLineWidth( int width );

    /**
     * Set the pen thickness for the ne-compass box.
     * @param width - the pen thickness used to draw the ne-compass.
     * @return - an error message if the pen thickness could not be set; an empty
     *      string otherwise.
     */
    QString setCompassNELineWidth( int width );

    /**
     * Set whether of not the image box should be visible.
     * @param visible - true if the image box should be visible; false, otherwise.
     */
    void setBoxVisible( bool visible);

    /**
     * Set whether of not the ne compass should be visible.
     * @param visible - true if the compass should be visible; false, otherwise.
     */
    void setCompassNEVisible( bool visible);

    /**
     * Set whether of not the xy compass should be visible.
     * @param visible - true if the compass should be visible; false, otherwise.
     */
    void setCompassXYVisible( bool visible);

    /**
     * Store the location of the main image rectangle.
     * @param br - one corner of the image rectangle in pixel coordinates.
     * @param tl - the opposite corner of the image rectangle in pixel coordinates.
     */
    void setImageRectangle( QPointF br, QPointF tl );

    /**
     * Set the index of the settings tab.
     * @param index - the index of the settings tab.
     * @return - an error message if the tab index could not be set; an empty string
     *      otherwise.
     */
    QString setTabIndex( int index );

    virtual ~ImageContext();

    const static QString CLASS_NAME;
    const static QString CURSOR_TEXT;


private slots:

    /**
     * Redraw the context image.
     */
    void _contextChanged();

private:

    /**
     * Constructor.
     * @param path - a base path identifier.
     * @param id - an object specific identifier.
     */
    ImageContext( const QString& path, const QString& id );
    class Factory;

    void _clearDraw();
    void _clearView();

    Controller* _getControllerSelected() const;

    enum ContextDraw { IMAGE_BOX,  ARROW_NE, ARROW_XY };

    QString _getPreferencesId() const;
    void _initializeDefaultPen( const QString& key, int red, int green,
            int blue, int alpha, int width );
    void _initializeDefaultState();
    void _initializeCallbacks();

    bool _setColor( const QString& key, const QString& majorKey,
            const QString& userId, int colorAmount, QString& errorMsg );
    QString _setDrawColor( int redValue, int greenValue, int blueValue,
            const QString& key, const QString& userID );
    QString _setLineWidth( const QString& key, const QString& userName, int width );
    void _setVisible( bool visible, const QString& key);
    void _updateImageView( const QPointF& topLeft, const QPointF& bottomRight );
    void _updateSelection( int mouseX, int mouseY );

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;
    Carta::State::StateInterface m_stateData;

    std::shared_ptr<DrawStackSynchronizer> m_contextDraw;

    std::unique_ptr<Settings> m_settings;

    static bool m_registered;

    bool m_mouseDown = false;


    const static QString CORNER_0;
    const static QString CORNER_1;
    const static QString BOX;
    const static QString BOX_SELECTED;
    const static QString BOX_VISIBLE;
    const static QString COMPASS_VISIBLE_XY;
    const static QString COMPASS_VISIBLE_NE;
    const static QString COMPASS_XY;
    const static QString COMPASS_NE;
    const static QString COORD_ROTATION;
    const static QString EAST;
    const static QString IMAGE_HEIGHT;
    const static QString IMAGE_WIDTH;
    const static QString MODE;
    const static QString NORTH;
    const static int PEN_FACTOR;
	ImageContext( const ImageContext& other);
	ImageContext& operator=( const ImageContext& other );
};
}
}
