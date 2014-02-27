/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#ifndef STARTUPPARAMETERS_H
#define STARTUPPARAMETERS_H

/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include <QString>
#include <map>

struct VisualizationStartupParameters
{
    QString title;
    QString path;
    QString id;
    QString stamp;
    bool error;
    QString errorString;

    // by default we construct parameters with error
    VisualizationStartupParameters();

    VisualizationStartupParameters & setError( const QString & str);

    VisualizationStartupParameters & clearError();

};


/**
 * @brief Determines startup parameters, either using the new method by making
 *        a rest call to vizManager? or using the old method, where the parameters
 *        are insecurely passed as url encoded parameters
 * @param pureWebParams - list of pureweb parameters
 * @param globalConfigFile - path to the global config file, e.g. ~/.sfviewer.config
 */
VisualizationStartupParameters
determineStartupParameters(
        const std::map< QString, QString> & pureWebParams
        );

#endif // STARTUPPARAMETERS_H
