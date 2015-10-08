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

class Animator : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    virtual QList<QString> getLinks()  const Q_DECL_OVERRIDE;

    /**
     * Add an animator of the given type.
     * @param type an identifier for the type of animator to add (channel, image, etc).
     * @param animId a return parameter which gets initialized with the id of the animator that
     *          was added.
     * @return an error message if there was an error; otherwise, false.
     */
    QString addAnimator( const QString& type, QString& animId );

    /**
     * Adds a link to this animator.
     * @param cartaObject the link that will be managed.
     * @return an error message if there was a problem adding the link; otherwise an empty string.
     */
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Clear current state..
     */
    void clear();

    /**
     * Returns the animator of the indicated type.
     * @param an identifier for the animation type.
     * @return the specified animator or a nullptr if no such animator exists.
     */
    AnimatorType* getAnimator( const QString& type );

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
     * Return a string representing the animator state of a particular type.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding animator state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    void changeChannelIndex( int index );

    void changeImageIndex( int selectedImage );

    /**
     * Force a state refresh.
     */
    virtual void refreshState() Q_DECL_OVERRIDE;

    /**
     * Removes a link to this animator.
     * @param cartaObject the link to remove.
     * @return an error message if the link could not be removed; an empty string otherwise.
     */
    virtual QString removeLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Remove the animator of the indicated type.
     * @param type an identifier for the type of animator to remove.
     * @return an error message if the animator was not removed; otherwise, an empty string.
     */
    QString removeAnimator( const QString& type );

    /**
     * Reset user preferences for the animator.
     * @param state- the animator's state.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Reset the data state for the animator (what images/channels are selected, etc).
     * @param state - the animator's data state.
     */
    virtual void resetStateData( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Get the number of images being managed by the animator.
     * @return the number of images being managed by the animator.
     */
    int getMaxImageCount() const;

    static const QString CLASS_NAME;

    virtual ~Animator();
protected:
    virtual QString getSnapType(CartaObject::SnapshotType snapType) const Q_DECL_OVERRIDE;

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
    QString _initAnimator( const QString& type, bool* newAnimator );

    void _resetAnimationParameters( int selectedImage );

    //Reset the state of an individual animator.
    void _resetStateAnimator( const Carta::State::StateInterface& state, const QString& key );

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    /// Individual animation types.
    QMap<QString, AnimatorType*> m_animators;

    static bool m_registered;

    Animator( const Animator& other);
    Animator& operator=( const Animator& other );
};
}
}
