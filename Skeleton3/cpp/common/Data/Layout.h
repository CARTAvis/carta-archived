/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

#include <QStringList>

namespace Carta {

namespace Data {

class Layout : public CartaObject {

public:
    /**
     * Clear the layout state.
     */
    void clear();

    /**
     * Set a predefined analysis layout.
     */
    void setLayoutAnalysis();

    /**
     * Set a custom layout.
     */
    void setLayoutCustom( int rows, int cols );

    void setPlugins( const QStringList& names);

    /**
     * Set a predefined layout displaying only a single image.
     */
    void setLayoutImage();
    virtual ~Layout();
    const static QString CLASS_NAME;
    static const QString LAYOUT;

private:
    void _initializeCommands();
    void _initializeDefaultState();
    bool _setPlugin( const QStringList& name );
    bool _setLayoutSize( int rows, int cols );

    static bool m_registered;
    Layout( const QString& path, const QString& id );

    class Factory;

    static const QString HIDDEN;
    static const QString LAYOUT_ROWS;
    static const QString LAYOUT_COLS;
    static const QString LAYOUT_PLUGINS;
    Layout( const Layout& other);
    Layout operator=( const Layout& other );
};
}
}
