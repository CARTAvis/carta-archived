/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include "AnimatorType.h"

class Controller;
class Selection;
class IConnector;

class Animator : public QObject, public CartaObject {

	Q_OBJECT

public:



    /**
     * Adds a Controller to this animator.
     * @param controller the DataController that will be managed.
     */
    void addController( const std::shared_ptr<Controller>& controller );

    /**
     * Returns the number of controllees linked to this Animator.
     * @return a count of objects controlled by the animator.
     */
    int getLinkCount() const;

    /**
     * Returns the identifier of the controllee linked to the Animator.
     * @param index a link index.
     * @param the path of the controller at the given index linked to this Animator;
     */
    QString getLinkId( int linkindex ) const;


    //Identifier for an image.
    const static QString IMAGE;
    //Identifier for a channel.
    static const QString CHANNEL;
    static const QString CLASS_NAME;

private slots:
	//Adjusts internal state based on the state in the child controllers.
	void _adjustStateController();
	void _imageIndexChanged( const QString& params );
	void _channelIndexChanged( const QString& params );

private:
    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    Animator( const QString& path, const QString & id );

    class Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Animator (path, id);
        }
    };

    static const QString LINK;
    static const QString ANIMATION_TYPE;

    void _adjustStateAnimatorTypes();
    int _getIndex( const std::shared_ptr<Controller>& controller );
    void _initializeState();
    void _initializeAnimators();
    void _initializeCallbacks();
    QString _initAnimator( const QString& type );
    QString _initializeAnimator( const QString& type );

    QString _removeAnimator( const QString& type );
    void _resetAnimationParameters();

	//Identifier for this animator.
	StateInterface m_state;

	//List of controllers managed by this animator.
	QList<std::shared_ptr<Controller> > m_controllers;


	//Individual animation types.
	QMap<QString, std::shared_ptr<AnimatorType> > m_animators;


	static bool m_registered;

	Animator( const Animator& other);
	Animator operator=( const Animator& other );
};
