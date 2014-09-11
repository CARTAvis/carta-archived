#pragma once

class IConnector; // #include "IConnector.h"
class QStringList;

/**
 * @brief The IPlatform class. Central place to define APIs that will be implemented
 * differently on desktop vs server side vs scripted viewer.
 */

class IPlatform {

public:

    /// \brief returns a connector specific to this platform.
    /// \return connector specific to this platform
    virtual IConnector * connector() = 0;

    /// \brief return an initial list of files to load
    /// On desktop this will probably come from command line, on server
    /// this will probably come somehow via url encoded arguments...
    virtual const QStringList & initialFileList() = 0;

    /// empty virtual destructor
    virtual ~IPlatform() {;}

};
