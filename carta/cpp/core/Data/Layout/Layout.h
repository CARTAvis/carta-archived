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

class LayoutNode;

class Layout : public QObject, public Carta::State::CartaObject {
    friend class ViewManager;

    Q_OBJECT

public:

    using CartaObject::resetState;

    /**
     * Add a new window at the given position in the layout.
     * @param nodeId - a list of one or more window identifiers where the window should be added.
     * @param position - an identifier for where the window should be added (top,bottom,etc).
     * @return an errorMsg or an empty string if there was not error.
     */
    QString addWindow( const QStringList& nodeId, const QString& position );

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
    QString getStateString( const QString& sessionId = "", SnapshotType type = SNAPSHOT_INFO ) const override;

    bool isLayoutDefault() const;

    /**
     * Returns true if the layout is the standard analysis layout; false otherwise.
     * @return true if the layout is a standard analysis layout; false otherwise.
     */
    bool isLayoutAnalysis() const;

    bool isLayoutImageComposite() const;

    /**
     * Returns true if the layout is the standard histogram analysis layout; false otherwise.
     * @return true if the layout is a standard histogram analysis layout; false otherwise.
     */
    bool isLayoutHistogramAnalysis() const;

    /**
     * Returns true if the layout is the standard image layout; false otherwise.
     * @return true if the layout is a standard image layout; false otherwise.
     */
    bool isLayoutImage() const;

    /**
     * Restore a saved layout.
     * @param savedState the layout state that should be restored.
     */
    void resetState( const Carta::State::StateInterface& savedState );

    void setLayoutDefault(bool cleanPluginList);

    void setLayoutHistogramAnalysis();

    /**
     * Set a predefined analysis layout.
     */
    void setLayoutAnalysis();

    /**
     * Set plugins for each of the views in the layout
     * @param names a list of plugin names.
     * @param useFirst - true if the first plugin in the list should be used; false if an
     *      attempt should be made to retain an existing plugin.
     * @return an error message if there was a problem setting the plugins; an empty
     *      string otherwise.
     */
    QString setPlugins( const QStringList& names, bool useFirst = false);

     /**
     * Set a layout showing widgets currently under development.
     */
    void setLayoutDeveloper();

    /**
     * Set a predefined layout displaying only a single image and a imageZoom and a imageContxt
     */
    void setLayoutImageComposite();

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
    int _getIndex( const QString& plugin, const QString& locationId ) const;
    QString _getPlugin( const QString& locationId ) const;
    int _getPluginCount( const QString& nodeType ) const;
    int _getPluginIndex( const QString& nodeId, const QString& pluginId ) const;
    void _initializeCommands();
    void _initializeDefaultState();
    void _initLayout( LayoutNode* root, int rowCount, int colCount );

    void _makeRoot( bool horizontal = true );

    /**
     * Remove the layout cell with the indicated id.
     * @param nodeId - an identifier for a layout cell.
     * @return an errorMessage if there was a problem removing the cell; an empty string otherwise.
     */
    QString _removeWindow( const QString& nodeId );
    bool _replaceRoot( LayoutNode* otherNode, const QString& childReplacement );
    /**
     * Set the list of plugins to be displayed.
     * @param name - the list of plugins to be displayed ordered in reading order.
     * @param useFirst - true if the first plugin in the list should be used; false if an attempt should
     *      be made to retain the existing plugin.
     */
    //Assume the list size matches the grid size.
    bool _setPlugin( const QStringList& name, bool useFirst );

    /**
     * Set the plugin for the layout cell identified by the nodeId.
     */
    bool _setPlugin( const QString& nodeId, const QString& nodeType );
    LayoutNode* _splitNode( int rowCount, int colCount,  bool horizontal );

    std::unique_ptr<LayoutNode> m_layoutRoot;


    static bool m_registered;
    Layout( const QString& path, const QString& id );

    class Factory;

    static const QString LAYOUT_ROWS;
    static const QString LAYOUT_COLS;
    static const QString LAYOUT_NODE;
    static const QString LAYOUT_PLUGINS;
    static const QString POSITION;
    static const QString TYPE_SELECTED;

    static const QString TYPE_IMAGE;
    static const QString TYPE_IMAGECOMPOSITE;

    static const QString TYPE_DEFAULT;
    static const QString TYPE_ANALYSIS; // LineAnalysis, profiler
    static const QString TYPE_HISTOGRAMANALYSIS;
    static const QString TYPE_CUSTOM;
    Layout( const Layout& other);
    Layout& operator=( const Layout& other );
};
}
}
