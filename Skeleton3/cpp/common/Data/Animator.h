/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include "Data/ILinkable.h"
#include "AnimatorType.h"
#include "Data/LinkableImpl.h"

class IConnector;

namespace Carta {

namespace Data {

class Controller;
class Selection;

class Animator : public QObject, public CartaObject, public ILinkable {

    Q_OBJECT

public:

    /**
     * Adds a Controller to this animator.
     * @param controller the DataController that will be managed.
     */
    virtual bool addLink( const std::shared_ptr<Controller>& controller ) ;

    /**
     * Adds a Controller to this animator.
     * @param controller the DataController that will be managed.
     */
    virtual bool removeLink( const std::shared_ptr<Controller>& controller );

    /**
     * Clear current state..
     */
    void clear();

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

    /**
     * Return a list of identifiers for all objects that are controlled by this animator.
     * @return a list of identifiers for objects under the control of this animator.
     */
    QList<QString> getLinks() const;

    static const QString CLASS_NAME;

    void changeChannelIndex( const QString& params );

private slots:
    //Adjusts internal state based on the state in the child controllers.
    void _adjustStateController( const Controller* controller);
    void _imageIndexChanged( const QString& params );
    void _channelIndexChanged( const QString& params );

private:
    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    Animator( const QString& path, const QString & id );

    class Factory;

    void _adjustStateAnimatorTypes();

    void _initializeState();
    void _initializeAnimators();
    void _initializeCallbacks();
    QString _initAnimator( const QString& type );
    QString _initializeAnimator( const QString& type );

    QString _removeAnimator( const QString& type );
    void _resetAnimationParameters( int selectedImage );

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    /// Individual animation types.
    QMap<QString, std::shared_ptr<AnimatorType> > m_animators;


    static bool m_registered;

    Animator( const Animator& other);
    Animator operator=( const Animator& other );
};
}
}
