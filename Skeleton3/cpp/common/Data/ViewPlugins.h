/***
 * Manages the data for a view displaying the plugins that have been loaded.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class ViewPlugins : public Carta::State::CartaObject {

public:

    virtual ~ViewPlugins();
    const static QString CLASS_NAME;

private:

    void _initializeDefaultState();
    void _insertPlugin( int ind, const QString& name, const QString& description,
                        const QString& type, const QString& version, const QString& errors );

    static bool m_registered;
    ViewPlugins( const QString& path, const QString& id );

    class Factory;

    static const QString PLUGINS;
    static const QString NAME;
    static const QString DESCRIPTION;
    static const QString TYPE;
    static const QString VERSION;
    static const QString ERRORS;
    static const QString STAMP;
    ViewPlugins( const ViewPlugins& other);
    ViewPlugins operator=( const ViewPlugins& other );
};
}
}
