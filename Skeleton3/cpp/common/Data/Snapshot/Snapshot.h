/***
 * The state of the application.
 *
 */

#pragma once

#include "State/StateInterface.h"


namespace Carta {

namespace Data {



class Snapshot {

public:

   Snapshot( const QString name = "");
   QString getName() const;
   QString toString() const;
   void setState( const QString& stateName, bool saved );
   void setCreatedDate( const QString& dateCreated );

    virtual ~Snapshot();

    const static QString DIR_LAYOUT;
    const static QString DIR_DATA;
    const static QString DIR_PREFERENCES;

private:
    const static QString DATE_CREATED;
    const static QString CLASS_NAME;

    void _initializeState( const QString& name );
    StateInterface m_state;

};


}
}
