#ifndef VOYAGERPLUGININTERFACE_H
#define VOYAGERPLUGININTERFACE_H

#include <QString>
#include <QtPlugin>

#include <vector>

class IVoyImage
{
public:
    virtual ~IVoyImage() {};
    virtual double operator()( const std::vector<int> & cursor) = 0;
    virtual std::vector<int> dims()= 0;
};

class IVoyImageReader : public IVoyImage
{
public:
    virtual QString name() const = 0;
};


class VoyagerPluginInterface
{
public:
    virtual ~VoyagerPluginInterface() {}
    virtual QString description() = 0;
    virtual double doMathOnImage( IVoyImage & image) = 0;
    virtual IVoyImageReader * getImageReader( const QString & fname) = 0;
};

Q_DECLARE_INTERFACE( IVoyImage, "org.voyageviewer.IVoyImage" )
Q_DECLARE_INTERFACE( VoyagerPluginInterface, "org.voyageviewer.plugininterface" )

#endif // VOYAGERPLUGININTERFACE_H
