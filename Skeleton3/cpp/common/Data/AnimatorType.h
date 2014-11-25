/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>

class Selection;


class AnimatorType : public QObject, public CartaObject {

    Q_OBJECT

public:

    /**
     * Sets the upper bound for the selection.
     * @param a nonnegative upper bound for the selection.
     */
    void setUpperBound( int value );

    /**
     * Returns a json string representing the state of this AnimatorType.
     * @return a Json string representing the state of this AnimatorType.
     */
    virtual QString getStateString() const;
    static const QString CLASS_NAME;
    static const QString ANIMATIONS;

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


    //Set state variables involving the animator
    void _saveState();

    QString m_animationType;
    static bool m_registered;

    //Animator's channel selection.
    std::shared_ptr<Selection> m_select;
    const static QString COMMAND_SET_FRAME;
    const static QString END_BEHAVIOR;
    const static QString RATE;
    const static QString STEP;
    AnimatorType( const AnimatorType& other);
    AnimatorType operator=( const AnimatorType& other );
};
