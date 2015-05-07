/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

#include <QStringList>
#include <QObject>

namespace Carta {

namespace Data {

class Layout : public QObject, public Carta::State::CartaObject {
    friend class ViewManager;

    Q_OBJECT

public:
    /**
     * Add a new window at the given position in the layout.
     * @param rowIndex the index of a row in the grid.
     * @param colIndex the index of a column in the grid.
     * @return an errorMsg or an empty string if there was not error.
     */
    QString addWindow( int rowIndex, int colIndex );

    /**
     * Clear the layout state.
     */
    void clear();

    /**
     * Returns a list of the current plugins in the view.
     * @return a list of view plugins.
     */
    QStringList getPluginList() const;

    /**
     * Return a string representing the layout state.
     * @return a QString representing the corresponding layout state.
     */
    QString getStateString() const;

    /**
     * Returns true if the layout is the standard analysis layout; false otherwise.
     * @return true if the layout is a standard analysis layout; false otherwise.
     */
    bool isLayoutAnalysis() const;

    /**
     * Returns true if the layout is the standard image layout; false otherwise.
     * @return true if the layout is a standard image layout; false otherwise.
     */
    bool isLayoutImage() const;

    /**
     * Remove the grid cell at the given row and column from the grid.
     * @param rowIndex the row of the cell to remove.
     * @param colIndex the column of the cell to remove.
     * @return an error message if there was a problem removing the cell; otherwise, and empty string.
     */
    QString removeWindow( int rowIndex, int colIndex );

    /**
     * Restore a saved layout.
     * @param savedState the layout state that should be restored.
     */
    void resetState( const Carta::State::StateInterface& savedState );

    /**
     * Set a predefined analysis layout.
     */
    void setLayoutAnalysis();

    /**
     * Set a layout showing widgets currently under development.
     */
    void setLayoutDeveloper();

    /**
     * Set a predefined layout displaying only a single image.
     */
    void setLayoutImage();

    /**
     * Set the number of rows and columns in the layout grid.
     * @param rows the number of rows in the grid.
     * @param cols the number of columns in the grid.
     * @param layoutType the name of one of the predefined layouts or a custom layout as the default.
     * @return a possible error message or an empty QString if there is no error.
     */
    QString setLayoutSize( int rows, int cols, const QString& layoutType = TYPE_CUSTOM );
    virtual ~Layout();
    const static QString CLASS_NAME;
    static const QString LAYOUT;


signals:
    /**
     * Notify that the plugins have changed.
     * @param newPlugins a list of the new plugins.
     * @param oldPlugins a list of the old plugins.
     */
    void pluginListChanged( const QStringList& newPlugins, const QStringList& oldPlugins );

private:

    int _getArrayIndex( int rowIndex, int colIndex ) const;
    QString _getPlugin( int rowIndex, int colIndex ) const;
    int _getColumnCount( int colIndex ) const;
    int _getMaxRowColumn() const;
    int _findEmptyRow( int colIndex, int targetRowIndex ) const;
    void _initializeCommands();
    void _initializeDefaultState();
    void _moveCell( int sourceRow, int sourceCol, int destRow, int destCol );
    /**
     * Set the list of plugins to be displayed.
     * @param name - the list of plugins to be displayed ordered in reading order.
     * @param custom - true if this is a custom layout false, if it is one of the recognized types.
     */
    //Assume the list size matches the grid size.
    bool _setPlugin( const QStringList& name, bool custom=false );
    bool _setPlugin( int rowIndex, int colIndex, const QString& name, bool insert = false );


    static bool m_registered;
    Layout( const QString& path, const QString& id );

    class Factory;

    static const QString COLUMN;
    static const QString EMPTY;
    static const QString HIDDEN;
    static const QString LAYOUT_ROWS;
    static const QString LAYOUT_COLS;
    static const QString LAYOUT_PLUGINS;
    static const QString ROW;
    static const QString TYPE_SELECTED;
    static const QString TYPE_IMAGE;
    static const QString TYPE_ANALYSIS;
    static const QString TYPE_CUSTOM;
    Layout( const Layout& other);
    Layout operator=( const Layout& other );
};
}
}
