/***
 * A layout cell that has a left and right child.
 *
 */

#pragma once


#include "LayoutNode.h"

#include <QStringList>

namespace Carta {

namespace Data {

class LayoutNode;

class LayoutNodeComposite : public LayoutNode {

public:
    /**
     * Add an empty layout cell to the cell identified by nodeId at the position indicated.
     * @param nodeId - an identifier for a layout cell that needs to be split.
     * @param position - an identifier for where the cell should be added (top, bottom, etc).
     * @param index - the index of the new empty window.
     * @return true if the layout cell was added; false otherwise.
     */
    virtual bool addWindow( const QString& nodeId, const QString& position, int index ) Q_DECL_OVERRIDE;

    virtual bool containsNode( const QString& nodeId ) const Q_DECL_OVERRIDE;
    virtual LayoutNode* findAncestor( const QStringList& nodeId, QString& childId ) Q_DECL_OVERRIDE;
    virtual bool getIndex( const QString& plugin, const QString& locationId, int* index ) const Q_DECL_OVERRIDE;
    virtual LayoutNode* getChildFirst() const  Q_DECL_OVERRIDE;
    virtual LayoutNode* getChildSecond() const  Q_DECL_OVERRIDE;
    virtual QString getPlugin( const QString& locationId ) const Q_DECL_OVERRIDE;

    virtual QStringList getPluginList() const Q_DECL_OVERRIDE;
    virtual QString getStateString() const Q_DECL_OVERRIDE;
    virtual bool isComposite() const Q_DECL_OVERRIDE;

    void releaseChild( const QString& key ) Q_DECL_OVERRIDE;
    virtual bool removeWindow( const QString& nodeId ) Q_DECL_OVERRIDE;
    virtual void resetState( const QString& state, QMap<QString,int>& usedPlugins ) Q_DECL_OVERRIDE;
    virtual void setHorizontal( bool horizontal ) Q_DECL_OVERRIDE;
    virtual void setChildFirst( LayoutNode* node ) Q_DECL_OVERRIDE;
    virtual void setChildSecond( LayoutNode* node ) Q_DECL_OVERRIDE;
    virtual bool setPlugin( const QString& nodeId, const QString& nodeType, int index ) Q_DECL_OVERRIDE;
    virtual bool setPlugins( QStringList& names, QMap<QString,int>& usedPlugins, bool useFirst ) Q_DECL_OVERRIDE;
    virtual QString setSize( int width, int height );
    virtual QString toString() const  Q_DECL_OVERRIDE;
    virtual ~LayoutNodeComposite();

    const static QString CLASS_NAME;;
    const static QString PLUGIN_LEFT;
    const static QString PLUGIN_RIGHT;

private:
    bool _addWindow( const QString& nodeId, const QString& position,
            const QString& childKey, std::unique_ptr<LayoutNode>& child, int index );
    QStringList _checkChild( LayoutNode* child, const QStringList & nodeIds ) const;
    LayoutNode* _findAncestorChild( const QStringList& nodeIds, QString& childId,
            LayoutNode* child );
    bool _removeWindow( const QString& nodeId,
            const QString& childKey, std::unique_ptr<LayoutNode>& child );
    void _initializeDefaultState();
    void _resetStateChild( const QString& childKey, std::unique_ptr<LayoutNode>& child,
            const Carta::State::StateInterface& newState, QMap<QString,int>& usedPlugins );
    void _setChild( const QString& key, std::unique_ptr<LayoutNode>& child, LayoutNode* node,
            bool destroy );
    void _updateChildState( const QString& childKey,  LayoutNode* child );

    std::unique_ptr<LayoutNode> m_firstChild;
    std::unique_ptr<LayoutNode> m_secondChild;

    class Factory;
    static bool m_registered;

    const static QString ORIENTATION;
    const static QString VERTICAL;
    const static QString HORIZONTAL;
    const static QString COMPOSITE;


    LayoutNodeComposite( const QString& path, const QString& id );

    LayoutNodeComposite( const LayoutNodeComposite& other);
    LayoutNodeComposite& operator=( const LayoutNodeComposite& other );
};
}
}
