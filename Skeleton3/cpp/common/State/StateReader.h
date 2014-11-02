/**
 * Restores the state from Json written to a file.
 */
#ifndef STATE_STATEREADER_H
#define STATE_STATEREADER_H

#include "StateInterface.h"
#include <QString>

class StateReader {

public:

    StateReader(const QString& fileName );

    /**
     * Returns true if the state was successfully read from the file; false otherwise.
     */
    bool restoreState();

    /**
     * Returns the state associated with the associated key.
     */
    QString getState( const QString& key ) const;

    /**
     * Returns a list of ( key, value) pairs for states associated with a view of a given
     * type.
     * @param type a QString representing a view type, for example, 'Controller'.
     * @return a pair of look-up keys and state values associated with views of the passed
     *      in type.
     */
    QList<std::pair<QString,QString> > getViews( const QString& type ) const;

private:
    QList<QString> _getViewObjectIds() const;
    QString m_fileName;
    StateInterface m_state;
};

#endif // STATE_STATEREADER_H

