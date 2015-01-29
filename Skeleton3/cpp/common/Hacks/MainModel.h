/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include <QObject>

namespace Hacks
{
namespace Model
{
/// container for all colormaps
class KnownColormaps : public QObject
{
    Q_OBJECT

    CLASS_BOILERPLATE( KnownColormaps);

public:

    KnownColormaps() { }

    virtual
    ~KnownColormaps() { }

    // add a colormap
    void
    add( Carta::Lib::PixelPipeline::IColormap::SharedPtr cmap, QString name );

    int
    count();

    Carta::Lib::PixelPipeline::IColormap::SharedPtr
    cmap( int ind );

    QString
    name( int ind );

signals:

    void
    updated();
};

class MainModel : public QObject
{
    Q_OBJECT

    CLASS_BOILERPLATE( MainModel );

public:

    explicit
    MainModel( QObject * parent = 0 );

    ~MainModel();

    std::shared_ptr < KnownColormaps >
    knownColormaps();

signals:

public slots:

private:

    Carta::Lib::PixelPipeline::IClippedPixelPipeline::SharedPtr m_ppipe = nullptr;
    QString m_statePrefix;
};
}

class GlobalsH {

    CLASS_BOILERPLATE( GlobalsH);
public:
    /// singleton pattern
    static GlobalsH * instance();

    /// get the main model
    Model::MainModel * mainModel();

private:
    static GlobalsH * m_instance;
    std::unique_ptr<Model::MainModel> m_mainModel = nullptr;
    GlobalsH();
};

}

Q_DECLARE_SMART_POINTER_METATYPE( std::shared_ptr )
