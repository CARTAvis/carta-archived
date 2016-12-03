/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include "core/State/StateInterface.h"
#include "core/State/ObjectManager.h"

namespace Carta {

namespace Data {

class Selection;

class AnimatorType : public QObject, public Carta::State::CartaObject {

    friend class Animator;

    Q_OBJECT

public:

    /**
     * Return the current selection index.
     * @return the current selection index.
     */
    int getFrame() const;

    /**
     * Returns a json string representing the user preferences.
     * @return a Json string representing user preferences.
     */
    QString getStatePreferences() const;

    /**
     * Returns a json string representing data selections.
     * @return a Json string representing data selections.
     */
    QString getStateData() const;

    QString getType() const;

    /**
     * Returns true if the animator is no longer visually available; false otherwise.
     * @return true if the animator is hidden; false otherwise.
     */
    bool isRemoved() const;
    bool isVisible() const;

    /**
     * Reset the animator's selections.
     * @param state - the selection state of the animator.
     */
    virtual void resetStateData( const QString& state ) Q_DECL_OVERRIDE;

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
     * Set the lower bound the user wants to use.
     * @param lowerBound - a nonnegative selection lower bound.
     * @param return an error message if the user lower bound could not be set;
     *      an empty string otherwise.
     */
    QString setLowerBoundUser( int lowerBound );



    /**
     * Sets the upper bound for the selection.
     * @param a nonnegative upper bound for the selection.
     */
    void setUpperBound( int value );

    /**
     * Sets the user upper bound (less than the upper bound).
     * @param upperBound - a nonnegative upper bound chosen by the user.
     * @return an error message if the user upper bound could not be set;
     *      an empty string otherwise.
     */
    QString setUpperBoundUser( int upperBound );

    /**
     * Show/hide the animator settings.
     * @param visible - true if the animator settings should be visible; false otherwise.
     */
    void setSettingsVisible( bool visible );

    /**
     * Set the animator visible/invisible.
     * @param visible - true if the animator should be visible; false if it should be hidden.
     */
    void setVisible( bool visible );
    void setRemoved( bool removed );

    static const QString CLASS_NAME;
    static const QString ANIMATIONS;
    static const QString SETTINGS_VISIBLE;

    virtual ~AnimatorType();

signals:
    void indexChanged(int,const QString&);

private slots:
    void _selectionChanged();

private:
    void _setType( const QString& type );
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

    static bool m_registered;

    //Animator's selection.
    Selection* m_select;
    QString m_type;
    const static QString COMMAND_SET_FRAME;
    const static QString END_BEHAVIOR;
    const static QString END_BEHAVIOR_WRAP;
    const static QString END_BEHAVIOR_JUMP;
    const static QString END_BEHAVIOR_REVERSE;
    const static QString RATE;
    const static QString STEP;

    bool m_visible;
    bool m_removed;
    AnimatorType( const AnimatorType& other);
    AnimatorType& operator=( const AnimatorType& other );
};
}
}
