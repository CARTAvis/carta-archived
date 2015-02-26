/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>

namespace Carta {

namespace Data {

class Selection;

class AnimatorType : public QObject, public CartaObject {

    Q_OBJECT

public:
    /**
     * Returns true if the animator is no longer visually available; false otherwise.
     * @return true if the animator is hidden; false otherwise.
     */
    bool isRemoved() const;

    /**
     * Sets the upper bound for the selection.
     * @param a nonnegative upper bound for the selection.
     */
    void setUpperBound( int value );

    /**
     * Set the current index of the selection.
     * @param index the current selection index.
     */
    void setIndex( int index );

    /**
     * Returns a json string representing the state of this AnimatorType.
     * @return a Json string representing the state of this AnimatorType.
     */
    virtual QString getStateString() const;

    /**
     * Set the animator visible/invisible.
     * @param removed true if the animator should be hidden; false otherwise.
     */
    void setRemoved( bool removed );
    static const QString CLASS_NAME;
    static const QString ANIMATIONS;

    virtual ~AnimatorType();

signals:
    void indexChanged(const QString& params);

private:

    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    AnimatorType( const QString& path, const QString& id );
    class Factory;

    //Initialize an individual animator's state (image, channel, etc).
    void _initializeState( /*StateKey keyStep, StateKey keyRate,
                                                          StateKey keyEnd, IConnector* connector*/ );

    //Add callbacks for commands.
    void _initializeCommands();

    QString _makeSelection();

    QString _setEndBehavior( const QString& params );
    QString _setFrameRate( const QString& params );
    QString _setFrameStep( const QString& params );

    //Set state variables involving the animator
    void _saveState();

    QString m_animationType;
    bool m_removed;
    static bool m_registered;

    //Animator's channel selection.
    Selection* m_select;
    const static QString COMMAND_SET_FRAME;
    const static QString END_BEHAVIOR;
    const static QString END_BEHAVIOR_WRAP;
    const static QString END_BEHAVIOR_JUMP;
    const static QString END_BEHAVIOR_REVERSE;
    const static QString RATE;
    const static QString STEP;
    AnimatorType( const AnimatorType& other);
    AnimatorType operator=( const AnimatorType& other );
};
}
}
