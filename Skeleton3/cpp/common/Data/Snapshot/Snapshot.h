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

   /**
    * Return the name of the snapshot.
    */
   QString getName() const;

   /**
    * Return snapshot information as a string.
    * @return snapshot information.
    */
   QString toString() const;

   /**
    * Set whether a particular type of snapshot was saved.
    * @param stateName - an identifier for a snapshot type ( layout, preferences, etc).
    * @param saved - true if the snapshot type was saved; false otherwise.
    */
   void setState( const QString& stateName, bool saved );

   /**
    * Set the date the snapshot was created.
    * @param dateCreated - the date the snapshot was created.
    */
   void setCreatedDate( const QString& dateCreated );
   /**
    * Set a long description of the snapshot.
    * @param descript - a descriptive information about the snapshot.
    */
   void setDescription( const QString& descript );

    virtual ~Snapshot();

    const static QString DIR_LAYOUT;
    const static QString DIR_DATA;
    const static QString DIR_PREFERENCES;

private:
    const static QString DESCRIPTION;
    const static QString DATE_CREATED;
    const static QString CLASS_NAME;

    void _initializeState( const QString& name );
    StateInterface m_state;

};


}
}
