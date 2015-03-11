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
     * Adds a link to this animator.
     * @param cartaObject the link that will be managed.
     */
    virtual bool addLink( CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Adds a link to this animator.
     * @param cartaObject the link to remove.
     */
    virtual bool removeLink( CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Clear current state..
     */
    void clear();

    /**
     * Force the connector to flush the state to the view.
     */
    void refreshState();

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

    void changeChannelIndex( int index );

    void changeImageIndex( int selectedImage );

    virtual ~Animator();



private slots:
    //Adjusts internal state based on the state in the child controllers.
    void _adjustStateController( Controller* controller);
    void _imageIndexChanged( int index );
    void _channelIndexChanged( int index );

private:
    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    Animator( const QString& path, const QString & id );

    class Factory;

    void _adjustStateAnimatorTypes();
    int _getMaxImageCount() const;
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
    QMap<QString, AnimatorType*> m_animators;


    static bool m_registered;

    Animator( const Animator& other);
    Animator operator=( const Animator& other );
};
}
}
