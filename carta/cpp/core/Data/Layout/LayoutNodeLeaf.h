/***
 * A layout cell that displays a single plug-in.
 *
 */

#pragma once

#include "LayoutNode.h"

#include <QStringList>

namespace Carta {

namespace Data {

class LayoutNodeLeaf : public LayoutNode {


public:
    virtual bool getIndex( const QString& plugin, const QString& locationId, int* index ) const Q_DECL_OVERRIDE;
    virtual QString getPlugin( const QString& locationId ) const Q_DECL_OVERRIDE;

    virtual QStringList getPluginList() const Q_DECL_OVERRIDE;
    virtual void resetState( const QString& state, QMap<QString,int>& usedPlugins) Q_DECL_OVERRIDE;
    virtual bool setPlugin( const QString& nodeId, const QString& nodeType, int index ) Q_DECL_OVERRIDE;
    virtual bool setPlugins( QStringList& names, QMap<QString,int>& usedPlugins, bool useFirst ) Q_DECL_OVERRIDE;
    /**
     * Set the type of plug-in that will be displayed by this cell.
     * @param name - an identifier for a plug-in type.
     */
    void setPluginType( const QString& name );

    /**
     * Return a string representation of the node.
     * @return - a string representation of the node.
     */
    virtual QString toString() const;
    virtual ~LayoutNodeLeaf();

    const static QString CLASS_NAME;

private:
    void _initializeCommands();
    void _initializeDefaultState();

    const static QString PLUGIN;


    static bool m_registered;
    LayoutNodeLeaf( const QString& path, const QString& id );

    class Factory;


    LayoutNodeLeaf( const LayoutNodeLeaf& other);
    LayoutNodeLeaf& operator=( const LayoutNodeLeaf& other );
};
}
}
