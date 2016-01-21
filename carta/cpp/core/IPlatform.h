#pragma once

class IConnector; // #include "IConnector.h"
class QStringList;
class QString;

/**
 * @brief The IPlatform class. Central place to define APIs that will be implemented
 * differently on desktop vs server side vs scripted viewer.
 */

class IPlatform
{
public:

    /// \brief returns a connector specific to this platform.
    /// \return connector specific to this platform
    virtual IConnector *
    connector() = 0;

    /// \brief return an initial list of files to load
    /// On desktop this will probably come from command line, on server
    /// this will probably come somehow via url encoded arguments...
    virtual const QStringList &
    initialFileList() = 0;

    /// Returns whether the code is running in an environment where security
    /// needs to be restricted, for example, access to the the file system.
    virtual bool
    isSecurityRestricted() const = 0;

    /// returns CARTA data directory this is where snapshots and images are stored
    /// On desktop this will be $HOME/CARTA on server /scratch
    virtual QString
    getCARTADirectory() = 0;

    /// empty virtual destructor
    virtual
    ~IPlatform() {; }
};
