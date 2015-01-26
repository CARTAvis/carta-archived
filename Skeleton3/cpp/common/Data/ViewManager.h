/***
 * Main class that manages the data state for the views.
 *
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"

namespace Carta {

namespace Data {

class Animator;
class Controller;
class DataLoader;
class Histogram;
class Colormap;
class Layout;
class ViewPlugins;

class ViewManager : public CartaObject {

public:

    virtual ~ViewManager(){}

    /**
     * Return the unique server side id of the object with the given name and index in the
     * layout.
     * @param pluginName an identifier for the kind of object.
     * @param index an index in the case where there is more than one object of the given kind
     *      in the layout.
     */
    QString getObjectId( const QString& pluginName, int index );

    /**
     * Link the color map with the given id to the controller with the given id.
     * @param colorId the unique server side id of the color map.
     * @param controlId the unique server side id of the controller whose display will change
     *      in response to color map changes.
     * @return true if the link was successfully established; false otherwise.
     */
    bool linkColoredView( const QString& colorId, const QString& controlId );

    /**
     * Load the file into the controller with the given id.
     * @param fileName a locater for the data to load.
     * @param objectId the unique server side id of the controller which is responsible for displaying
     *      the file.
     */
    void loadFile( const QString& objectId, const QString& fileName);

    /**
     * Reset the layout to a predefined analysis view.
     */
    void setAnalysisView();

    /**
     * Change the color map to the map with the given name.
     * @param colormapId the unique server-side id of a Colormap object.
     * @param colormapName a unique identifier for the color map to be displayed.
     */
    bool setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Reset the layout to a predefined view displaying only a single image.
     */
    void setImageView();

    static const QString CLASS_NAME;

private:
    ViewManager( const QString& path, const QString& id);
    class Factory;

    void _clear();

    int _findColorMap( const QString& id ) const;
    int _findController( const QString& id ) const;

    void _initCallbacks();

    void _initializeExistingAnimationLinks( int index );
    //Sets up a default set of states for constructing the UI if the user
    //has not saved one.
    void _initializeDefaultState();

    bool _linkColoredView( std::shared_ptr<Colormap> colorMap, std::shared_ptr<Controller> controller );

    QString _makeAnimator();
    QString _makeLayout();
    QString _makePluginList();
    QString _makeController();
    QString _makeHistogram();
    QString _makeColorMap();
    void _makeDataLoader();


    bool _readState( const QString& fileName );
    bool _saveState( const QString& fileName );

    //A list of Controllers requested by the client.
    QList <std::shared_ptr<Controller> > m_controllers;

    //A list of Animators requested by the client.
    QList < std::shared_ptr<Animator> > m_animators;

    //Colormap
    QList<std::shared_ptr<Colormap>  >m_colormaps;

    //Histogram
    QList<std::shared_ptr<Histogram> >m_histograms;

    static bool m_registered;
    std::shared_ptr<Layout> m_layout;
    std::shared_ptr<DataLoader> m_dataLoader;
    std::shared_ptr<ViewPlugins> m_pluginsLoaded;

    ViewManager( const ViewManager& other);
    ViewManager operator=( const ViewManager& other );
};

}
}
