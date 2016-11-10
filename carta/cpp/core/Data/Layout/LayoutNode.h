/***
 * A generic layout cell.
 *
 */

#pragma once

#include "State/ObjectManager.h"

#include <QStringList>

namespace Carta {

namespace Data {

class LayoutNode : public Carta::State::CartaObject {

public:

    /**
     * Add an empty layout cell to the cell identified by nodeId at the position indicated.
     * @param nodeId - an identifier for a layout cell that needs to be split.
     * @param position - an identifier for where the cell should be added (top, bottom, etc).
     * @param index - the index of the new empty window.
     * @return true if the layout cell was added; false otherwise.
     */
    virtual bool addWindow( const QString& nodeId, const QString& position, int index);

    /**
     * Returns true if this node or one of its descendents contains a layout cell with the
     * given nodeId; false otherwise.
     * @param nodeId - an identifier for a node.
     * @return true if the node is a descendent of this node; false otherwise.
     */
    virtual bool containsNode( const QString& nodeId ) const;

    /**
     * Returns the lowest layout cell that contains all the nodeIds in the list; sets the
     * childId to the identifier of the child containing the nodes.
     * @param nodeId - a list of node identifiers.
     * @param childId - a node identifier
     * @return the lowest level ancestor containing all the layout cells in the list.
     */
    virtual LayoutNode* findAncestor( const QStringList& nodeId, QString& childId );

    virtual QString getPlugin( const QString& locationId ) const = 0;

    /**
     * Returns the list of plugins displayed by this LayoutNode and its
     * descendents.
     * @return a list of plugin names displayed by this LayoutNode and its descendents.
     */
    virtual QStringList getPluginList() const = 0;

    /**
     * Return a pointer to the first child of this cell or a nullptr if this cell is childless.
     * @return the first child cell of this cell or a nullptr if there is no child.
     */
    virtual LayoutNode* getChildFirst() const;

    /**
     * Return a pointer to the second child of this cell or a nullptr if this cell is childless.
     * @return the second child cell of this cell or a nullptr if there is no child.
     */
    virtual LayoutNode* getChildSecond() const;

    /**
     * Returns the height of the plugin.
     * @return - the height of the plugin.
     */
    int getHeight() const;

    /**
     * Returns true if the layout cell with the locationId is either this cell or one of its descendents;
     * sets the index to the index of the located cell in depth-first search order among those displaying
     * the indicated plug-in.
     * @param plugin - the type of plug-in that is being indexed.
     * @param locationId - the identifier for a layout cell.
     * @param index - a modified parameter that is the index of the designated cell in depth-first
     *      search order or -1 if no such cell was found.
     * @return true if the cell was located; false otherwise.
     *
     */
    virtual bool getIndex( const QString& plugin, const QString& locationId, int* index ) const = 0;

    /**
     * Returns a string representation of this layout cell's state.
     * @return a string representation of the layout cell.
     */
    virtual QString getStateString() const;

    /**
     * Returns the width of the plugin.
     * @return - the width of the plugin.
     */
    int getWidth() const;

    /**
-     * Returns whether or not this is a composite layout cell (contains children).
-     * @return true if this layout node is composite; false otherwise.
-     */
    virtual bool isComposite() const;
    virtual void releaseChild( const QString& key );
    /**
     * Remove the layout cell with the given nodeId.
     * @param nodeId - an identifier for the layout cell to remove.
     * @return true if the layout cell was removed by this node or its children; false otherwise.
     */
    virtual bool removeWindow( const QString& nodeId );

    /**
        * Reset the animator's selections.
        * @param state - the selection state of the animator.
        */
    virtual void resetState( const QString& state, QMap<QString,int>& usedPlugins );

    /**
     * Set the first child of this node (ignored for leaf nodes).
     * @param node - the layout cell that will become the first child of this one.
     */
    virtual void setChildFirst( LayoutNode* node );

    /**
     * Set the second child of this node (ignored for leaf nodes).
     * @param node - the layout cell that will become the second child of this one.
     */
    virtual void setChildSecond( LayoutNode* node );

    /**
     * Set the vertical/horizontal orientation of composite nodes.
     * @param horizontal - true if the layout node stretches its children out horizontally; false for
     *      vertical alignment.
     */
    virtual void setHorizontal( bool horizontal );


    /**
     * Set the list of plug-ins that will be displayed by this LayoutNode and its children.
     * @param names - the list of plug-ins to be displayed by this LayoutNode and its children.
     * @param usedPlugins - used to update the index of the cell in the case of multiple cells displaying
     *      the same plugin.
     * @return true if the list of plug-ins were correctly set; false if there was an error setting them.
     */
    virtual bool setPlugins( QStringList& names, QMap<QString,int>& usedPlugins, bool useFirst ) = 0;

    /**
     * Set the plugin and index for the node with the given identifier.
     * @param nodeId - an identifier for a layout cell.
     * @param pluginType - an identifier for a plugin.
     * @param index - the index of the plugin when there are multiple windows displaying the same plugin.
     */
    virtual bool setPlugin( const QString& nodeId, const QString& pluginType, int index ) = 0;

    /**
     * Set the size of the layout cell.
     * @param width - the width of the layout cell.
     * @param height - the height of the layout cell.
     */
    virtual QString setSize( int width, int height );

    /**
     * Return a string representation of the state of this cell and its descendents.
     * @return a string representation of this layout and its descendents.
     */
    virtual QString toString() const;

    virtual ~LayoutNode();

protected:

    LayoutNode( const QString& className, const QString& path, const QString& id );

private:
    void _initializeCommands();
    void _initializeDefaultState();


    LayoutNode( const LayoutNode& other);
    LayoutNode& operator=( const LayoutNode& other );
};
}
}
