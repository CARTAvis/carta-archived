/***
 * Meshes together data, selection(s), and view(s).
 */

#pragma once

#include <memory>
#include "CoordinateFormatter.h"
#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include <QString>
#include <QList>
#include <QObject>

class ImageView;
class DataSource;
class Selection;
class CoordinateFormatterInterface;

class Controller: public QObject, public CartaObject {

    Q_OBJECT

public:
    /**
     * Clear the view.
     */
    void clear();

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
     * Returns the state associated with the key.
     * @param key a look up for a specific state.
     */
    int getState( const QString& type, const QString& key );

    /**
     * Save the state of this controller.
     */
    void saveState();

    /**
     * Returns a json string representing the state of this controller.
     * @return a string representing the state of this controller.
     */
    virtual QString getStateString() const;

    virtual ~Controller();

    static const QString CLASS_NAME;
    static const QString CURSOR;

signals:
    /**
     *  Notification that the data/selection managed by this controller has
     *  changed.
     */
    void dataChanged();

private slots:
    //Refresh the view based on the latest data selection information.
    void _loadView( bool forceReload = false );

private:

    /**
     *  Constructor.
     */
    Controller( const QString& id, const QString& path );

    class Factory : public CartaObjectFactory {

     public:

         CartaObject * create (const QString & path, const QString & id)
         {
             return new Controller (path, id);
         }
     };

    //Provide default values for state.
    void _initializeState();
    void _initializeCallbacks();
    void _initializeSelections();
    void _initializeSelection( std::shared_ptr<Selection> & selection );

    void _updateCursor();

    static bool m_registered;

    static const QString CLIP_VALUE;
    static const QString AUTO_CLIP;
    static const QString DATA_COUNT;
    static const QString DATA_PATH;

    //Data Selections
    std::shared_ptr<Selection> m_selectChannel;
    std::shared_ptr<Selection> m_selectImage;

    //Data View
    std::shared_ptr<ImageView> m_view;

    //Data available to and managed by this controller.
    QList<std::shared_ptr<DataSource> > m_datas;

    /// coordinate formatter
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter;

    Controller(const Controller& other);
    Controller operator=(const Controller& other);

};
