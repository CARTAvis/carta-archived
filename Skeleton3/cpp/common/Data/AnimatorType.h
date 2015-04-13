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
     * Return the current selection index.
     * @return the current selection index.
     */
    int getFrame() const;

    /**
     * Returns a json string representing the state of this AnimatorType.
     * @return a Json string representing the state of this AnimatorType.
     */
    //virtual QString getStateString() const;

    /**
     * Returns true if the animator is no longer visually available; false otherwise.
     * @return true if the animator is hidden; false otherwise.
     */
    bool isRemoved() const;

    /**
     * Set whether the animator should wrap, reverse, etc when it gets to the start
     * or end channel.
     * @param an identifier for the animation behavior at the ends of the range.
     */
    QString setEndBehavior( const QString& endBehavior );

    /**
     * Set the current index of the selection.
     * @param frameIndex the current selection index.
     * @return an error message if there is one; otherwise an empty string;
     */
    QString setFrame( int frameIndex );

    /**
     * Set the animation speed.
     * @param rate a positive rate indicator.
     */
    QString setFrameRate( int rate );

    /**
     * Set the number of frames to increment/decrement each time.
     * @param stepSize a positive integer( normally 1) indicating the number of steps to increment.
     */
    QString setFrameStep( int stepSize );

    /**
     * Set the animator visible/invisible.
     * @param removed true if the animator should be hidden; false otherwise.
     */
    void setRemoved( bool removed );

    /**
     * Sets the upper bound for the selection.
     * @param a nonnegative upper bound for the selection.
     */
    void setUpperBound( int value );

    /**
     * Set the purpose of the animator - whether it is animating channels, images, etc.
     */
    void setPurpose( const QString& name );

    static const QString CLASS_NAME;
    static const QString ANIMATIONS;

    QString changeIndex( const QString & params );

    virtual ~AnimatorType();

signals:
    void indexChanged(int);

private:

    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    AnimatorType( const QString& path, const QString& id );
    class Factory;

    //Initialize an individual animator's state (image, channel, etc).
    void _initializeState( );

    //Add callbacks for commands.
    void _initializeCommands();

    QString _makeSelection();

    //Set state variables involving the animator
    void _saveState();

    bool m_removed;
    static bool m_registered;

    //Animator's selection.
    Selection* m_select;
    const static QString COMMAND_SET_FRAME;
    const static QString END_BEHAVIOR;
    const static QString END_BEHAVIOR_WRAP;
    const static QString END_BEHAVIOR_JUMP;
    const static QString END_BEHAVIOR_REVERSE;
    const static QString LABEL;
    const static QString RATE;
    const static QString STEP;
    AnimatorType( const AnimatorType& other);
    AnimatorType operator=( const AnimatorType& other );
};
}
}
