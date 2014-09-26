/***
 * Meshes together data, selection(s), and view(s).
 */

#pragma once

#include <memory>
#include <State/StateKey.h>
#include <QString>
#include <QList>
#include <QObject>

class TestView;
class DataSource;
class DataSelection;

class DataController: public QObject {

    Q_OBJECT

public:

    /**
     *  Constructor.
     */
    DataController();

    /**
     * Set an identifier for this DataController.
     * @param winId a unique identifier.
     */
    void setId(const QString& winId);

    /**
     * Add data to this controller.
     * @param fileName the location of the data; 
     * 			this could represent a url or an absolute path on a local filesystem.
     */
    void addData(const QString& fileName);

    /**
     * Make a channel selection.
     * @param val a String representing a channel selection.
     */
    void setFrameChannel(const QString& val);

    /**
     *  Make a data selection.
     *  @param val a String representing the index of a specific data selection.
     */
    void setFrameImage(const QString& val);

    /**
     * Returns the unique identifer for this DataController.
     * @return a String that uniquely identifies this controller.
     */
    QString getId() const;

    /**
     * Returns the state associated with the key.
     * @param key a look up for a specific state.
     */
    QString getState(StateKey key) const;

    /**
     * Save the state of this controller.
     */
    void saveState();

    virtual ~DataController();

    signals:
    /**
     *  Notification that the data/selection managed by this controller has
     *  changed.
     */
    void dataChanged();

private:

    //Add state callbacks.
    void _initializeCommands();

    //Refresh the view based on the latest data selection information.
    void _loadView( bool forceReload );

    //Adds data based on state variables defining data sources.
    void _loadData( int dataCount );

    //Restore the state of the DataController based on state variables.
    void _restoreState();

    //Data Selections
    std::shared_ptr<DataSelection> m_selectChannel;
    std::shared_ptr<DataSelection> m_selectImage;

    //Data View
    std::shared_ptr<TestView> m_view;

    //Data available to and managed by this controller.
    QList<std::shared_ptr<DataSource> > m_datas;

    //Unique identifier
    QString m_winId;

    DataController(const DataController& other);
    DataController operator=(const DataController& other);

};
