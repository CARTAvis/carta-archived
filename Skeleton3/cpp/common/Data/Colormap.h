/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"


class Colormap : public CartaObject {

public:

    virtual ~Colormap();
    const static QString CLASS_NAME;
private:
    void _initializeDefaultState();


    static bool m_registered;
    Colormap( const QString& path, const QString& id );

    class Factory : public CartaObjectFactory {


    public:

        Factory():
            CartaObjectFactory(){};

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Colormap (path, id);
        }
    };

	Colormap( const Colormap& other);
	Colormap operator=( const Colormap& other );
};
