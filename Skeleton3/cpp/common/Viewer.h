/***
 * Implementation of the sample application.
 *
 */

#pragma once

#include <QObject>
#include <QList>
#include <memory>

class DataAnimator;
class DataController;
class ScriptedCommandListener;

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

protected:

    ScriptedCommandListener * m_scl = nullptr;

private:
    //Utility function that parses a string of the form:  key1:value1,key2:value2,etc for
    //keys contained in the QList and returns a vector containing their corresponding values.
    QVector<QString> _parseParamMap( const QString& params, const QList<QString> & keys );

    //A map of DataControllers requested by the client; keys are their unique identifiers.
    std::map <QString, std::shared_ptr<DataController> > m_dataControllers;

    //A map of DataAnimators requested by the client;  keys are their unique identifiers.
    std::map <QString, std::shared_ptr<DataAnimator> > m_dataAnimators;
};
