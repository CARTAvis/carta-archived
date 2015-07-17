/***
 * Creates layout cells of the appropriate type.
 *
 */

#pragma once
#include <QString>

namespace Carta {

namespace Data {

class LayoutNode;

class NodeFactory {


public:
    static const QString EMPTY;
    static const QString HIDDEN;
    static const QString POSITION_TOP;
    static const QString POSITION_BOTTOM;
    static const QString POSITION_LEFT;
    static const QString POSITION_RIGHT;

    /**
     * Makes a composite layout node capable of having children.
     * @param horizontal true if the node arranges its children horizontally; false otherwise.
     * @return a pointer to the node that was created; caller is responsible for destruction.
     */
    static LayoutNode* makeComposite( bool horizontal );

    /**
     * Makes a leaf layout node capable of displaying a plug-in.
     * @param name - an identifier for the plug-in to be displayed; default is an empty cell.
     * @return a pointer to the node that was created; caller is responsible for destruction.
     */
    static LayoutNode* makeLeaf( const QString& name = EMPTY);

private:
    NodeFactory();
    ~NodeFactory();
    NodeFactory( const NodeFactory& other);
    NodeFactory& operator=( const NodeFactory& other );
};
}
}
