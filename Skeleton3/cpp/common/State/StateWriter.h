/**
 *
 **/


#ifndef STATE_WRITER_H
#define STATE_WRITER_H

#include "common/State/StateInterface.h"


class StateWriter {

public:

    /// constructor
    StateWriter( const QString& filePath );

    /**
     * Add additional state for writing.
     * @param path a QString key for where the state should be located.
     * @param data a QString representing the state that should be added.
     */
    void addPathData( const QString& path, const QString& data );

    /**
     * Save the state.
     * @return true if the state was successfully saved; false otherwise.
     */
    bool saveState();

private:
    StateInterface m_state;
    QString m_fileLocation;
};


#endif // STATE_WRITER_H
