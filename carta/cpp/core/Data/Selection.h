/***
 * Handles stores/retrieve state involving an index that is bounded both below and above;
 * the bounds and index are assumed to be nonnegative integers.
 */

#pragma once

#include <QString>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include <QObject>

namespace Carta {

namespace Data {

class Selection : public QObject, public Carta::State::CartaObject {

    Q_OBJECT

public:

    //GENERAL NOTE:  LowerBound <= UserLowerBound<=Index<=UserUpperBound<=UpperBound - 1

    /**
     * Returns the current index selection;
     */
    int getIndex() const;

    /**
     * Returns the lower bound for the selection.
     * @return the lowest possible selection value.
     */
    int getLowerBound() const;

    /**
     * Return the user settable lower bound for the selection.
     * @return the user set lowest possible selection value.
     */
    int getLowerBoundUser() const;


    /**
     * Returns a json string representing the state of this selection.
     * @return a string representing the state of this selection.
     */
    QString getStateString() const;

    /**
     * Returns the upper bound for the selection.
     * @return the number of selections that can be made.
     */
    int getUpperBound() const;

    /**
     * Returns the user settable upper bound for the selection.
     * @return the user settable upper bound for selections.
     */
    int getUpperBoundUser() const;

    /**
     * Sets the current index of the selection, provided it is in the selection range.
     * @param val the new selection index.
     * @return an error message if the index was not set; otherwise and empty string.
     */
    QString setIndex( int val);

    /**
     * Sets the lower bound of the selection.
     * @param newLowerBound a nonnegative integer representing the new selection lower bound.
     */
    void setLowerBound(int newLowerBound);

    /**
     * Sets the user's lower bound limit on selections.
     * @param userLowerBound - a nonnegative value representing the user's preferences for
     *      selection lower bounds.
     * @return an empty string if the user lower bound was successfully set; an empty string
     *      otherwise.
     */
    QString setLowerBoundUser( int userLowerBound);

    /**
     * Sets the upper bound of the selection.
     * @param newUpperBound a nonnegative integer representing the new selection upper bound.
     */
    void setUpperBound(int newUpperBound);

    /**
     * Sets the user's preferred upper bound for the selection.
     * @param userUpperBound - a nonnegative integer which is the user's preferred upper bound.
     * @return an error message if the user upper bound could not be set; an empty string
     *      otherwise.
     */
    QString setUpperBoundUser( int userUpperBound );


    virtual ~Selection();

    //Bounds and index for the state.
    static const QString HIGH_KEY;
    static const QString HIGH_KEY_USER;
    static const QString INDEX_KEY;
    static const QString LOW_KEY;
    static const QString LOW_KEY_USER;
    static const QString CLASS_NAME;
    static const QString SELECTIONS;
    //Identifier for an image.
    const static QString IMAGE;
    const static QString REGION;
    //Identifier for a channel.
    static const QString CHANNEL;

    static bool m_registered;

signals:
    void indexChanged();

private:

    /**
     * Constructor.
     * @param prefix a QString identifying what is being selected (image range, channel range, etc).
     * @param identifier a QString identifying the specific window where this selection applies.
     */
    Selection( const QString& prefix, const QString& identifier);

    class Factory : public Carta::State::CartaObjectFactory {

    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Selection (path, id);
        }
    };

    void _initializeCallbacks();

    //Set initial values of the state if they do not already exist.
    void _initializeStates();

    //Returns the value of the passed in key as an integer.
    int _getValue(const QString& key) const;

    Selection( const Selection& other);
    Selection& operator=( const Selection& other );
};
}
}
