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

    /**
     * Save a layout snapshot.
     * @param layoutStr - the layout snapshot to save.
     * @return an error message if the snapshot could not be saved; otherwise an empty string.
     */
    QString saveLayout(const QString& layoutStr) const;

    /**
     * Save a preferences snapshot.
     * @param prefStr - the preferences snapshot to save.
     * @return an error message if the preferences could not be saved; otherwise an empty string.
     */
    QString savePreferences(const QString& prefStr) const;

    /**
     * Save a data snapshot.
     * @param dataStr - the data snapshot to save.
     * @return an error message if the data snapshot could not be saved; otherwise and empty string.
     */
    QString saveData(const QString& dataStr) const;

private:
    StateInterface m_state;
    QString m_fileLocation;
};


#endif // STATE_WRITER_H
