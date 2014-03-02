#ifndef VOYPLUG_H
#define VOYPLUG_H

#include "../../Viewer/VoyagerPluginInterface.h"
#include <QObject>
#include <QString>


class VoyPlug : public QObject, public VoyagerPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.voyageviewer.plugininterface" FILE "VoyagerViewer.json")
    Q_INTERFACES( VoyagerPluginInterface)

public:
    VoyPlug(QObject *parent = 0);

    // VoyagerPluginInterface interface
public:
    virtual QString description();
    virtual double doMathOnImage(IVoyImage &image);
    virtual IVoyImageReader * getImageReader(const QString & /*fname*/) { return 0; }
};

#endif // VOYPLUG_H
