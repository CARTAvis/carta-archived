/***
 * Implementation of the sample application.
 *
 */

#pragma once

#include "CartaLib/IImage.h"
#include <QObject>
#include <QList>
#include <memory>

class DataAnimator;
class DataController;
class ScriptedCommandListener;
class RawView2QImageConverter;
class IConnector;

class Viewer : public QObject
{

    Q_OBJECT

public:

    /// constructor - does not need to do anything
    /// it gets called/constructed by skeleton when connector is not yet connected, but platform is
    /// functioning
    explicit Viewer();

    /// called by skeleton when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void start();

signals:

public slots:


protected slots:

    /// internal callback for scripted commands
    void scriptedCommandCB(QString command);

    void reloadFrame(bool forceClipRecompute = false);

    void mouseCB( const QString & path, const QString & val);

protected:

    /// pointer to scripted command listener
    ScriptedCommandListener * m_scl = nullptr;

    /// pointer to the loaded image
    Image::ImageInterface::SharedPtr m_image = nullptr;

    /// current frame
    int m_currentFrame = -1;

    /// are we recomputing clip on frame change?
    bool m_clipRecompute = true;

    /// pointer to the rendering algorithm
    std::shared_ptr<RawView2QImageConverter> m_rawView2QImageConverter;

private:
    //Utility function that parses a string of the form:  key1:value1,key2:value2,etc for
    //keys contained in the QList and returns a vector containing their corresponding values.
//    QVector<QString> _parseParamMap( const QString& params, const QList<QString> & keys );

    //Sets up a default set of states for constructing the UI if the user
    //has not saved one.
    void initializeDefaultState();

    //A map of DataControllers requested by the client; keys are their unique identifiers.
    std::map <QString, std::shared_ptr<DataController> > m_dataControllers;

    //A map of DataAnimators requested by the client;  keys are their unique identifiers.
    std::map <QString, std::shared_ptr<DataAnimator> > m_dataAnimators;


    /// pointer to connector
    IConnector * m_connector;

    /// coordinate formatter
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter;
};

