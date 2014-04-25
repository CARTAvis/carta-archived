#ifndef COMMON_IPLATFORM_H_
#define COMMON_IPLATFORM_H_

//#include "IConnector.h"

class IConnector;

/**
 * @brief The IPlatform class. Central place to define APIs that will be different on desktop
 * vs server side vs scripted viewer.
 */

class IPlatform {

public:

    ///
    /// \brief returns a connector specific to this platform.
    /// \return connector specific to this platform
    ///
    virtual IConnector * connector() = 0;

    virtual ~IPlatform() {}

};

#endif // COMMON_IPLATFORM_H_

