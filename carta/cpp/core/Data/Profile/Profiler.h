/***
 * Manages Profiler settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "CartaLib/IImage.h"
#include "ProfileExtractor.h"

#include <QObject>

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
}
}


class ImageView;


namespace Carta {
namespace Plot2D {
class Plot2DGenerator;
}

namespace Data {

class Plot2DManager;
class Controller;
class LinkableImpl;
class Settings;

class Profiler : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;


    /**
     * Set the drawing style for the Profiler (outline, filled, etc).
     * @param style a unique identifier for a Profiler drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QString setGraphStyle( const QString& style );

    /**
     * Set whether or not the vertical axis should use a log scale.
     * @param logCount true if the vertical axis should be logarithmic; false otherwise.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setLogCount( bool logCount );

    /**
    * Determine whether or not the vertical axis is using a log scale.
    * @return true if the vertical axis is using a log scale; false otherwise.
    */
    bool getLogCount();



    virtual ~Profiler();
    const static QString CLASS_NAME;


private slots:

    void _createProfiler( Controller* );


private:

    void  _generateProfile( bool newDataNeeded, Controller* controller=nullptr);
    Controller* _getControllerSelected() const;
    void _loadProfile( Controller* controller);


    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface>> _generateData(Controller* controller);
    
    /**
     * Returns the server side id of the Profiler user preferences.
     * @return the unique server side id of the user preferences.
     */
    QString _getPreferencesId() const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();


    static bool m_registered;


    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Profiler( const QString& path, const QString& id );
    class Factory;


    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Preferences
    std::unique_ptr<Settings> m_preferences;

    std::unique_ptr<Plot2DManager> m_plotManager;

	Profiler( const Profiler& other);
	Profiler operator=( const Profiler& other );
};
}
}
