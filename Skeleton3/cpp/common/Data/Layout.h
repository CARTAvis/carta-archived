/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

#include <QStringList>


class Layout : public CartaObject {

public:
    /**
     * Clear the layout.
     */
    void clear();
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

    class Factory : public CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( LAYOUT ){};

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Layout (path, id);
        }
    };

    static const QString LAYOUT_ROWS;
    static const QString LAYOUT_COLS;
    static const QString LAYOUT_PLUGINS;
	Layout( const Layout& other);
	Layout operator=( const Layout& other );
};
