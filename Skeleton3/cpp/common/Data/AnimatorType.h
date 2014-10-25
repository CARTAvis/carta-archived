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

    //Set the upper bound for the selection.
    void setUpperBound( int value );
    static const QString CLASS_NAME;

signals:
    void indexChanged(const QString& params);

private:

    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    AnimatorType( const QString& path, const QString& id );
    class Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new AnimatorType (path, id);
        }
    };

	//Initialize an individual animator's state (image, channel, etc).
	void _initializeState( /*StateKey keyStep, StateKey keyRate,
	        StateKey keyEnd, IConnector* connector*/ );

	//Add callbacks for commands.
	void _initializeCommands();


	//Set state variables involving the animator
	void _saveState();

	QString m_animationType;
	StateInterface m_state;
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
