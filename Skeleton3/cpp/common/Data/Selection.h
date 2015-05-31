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

    /**
     * Return the value of the value of the state corresponding to the key.
     * @param key a lookup key for the state.
     * @return the value of the state.
     */
     int getState(const QString& key ) const;

     /**
      * Returns a json string representing the state of this selection.
      * @return a string representing the state of this selection.
      */
     QString getStateString() const;

    /**
     * Returns the current index selection;
     */
    int getIndex() const;

    /**
     * Returns the lower bound for the selection.
     */
    int getLowerBound() const;

    /**
     * Returns the upper bound for the selection.
     */
    int getUpperBound() const;


    /**
     * Sets the current index of the selection, provided it is in the selection range.
     * @param val the new selection index.
     * @return an error message if the index was not set; otherwise and empty string.
     */
    QString setIndex( int val);

    /**
     * Sets the upper bound of the selection.
     * @param newUpperBound a nonnegative integer representing the new selection upper bound.
     */
    void setUpperBound(int newUpperBound);

    /**
     * Sets the lower bound of the selection.
     * @param newLowerBound a nonnegative integer representing the new selection lower bound.
     */
    void setLowerBound(int newLowerBound);

    virtual ~Selection();

    //Bounds and index for the state.
    static const QString HIGH_KEY;
    static const QString INDEX_KEY;
    static const QString LOW_KEY;
    static const QString CLASS_NAME;
    static const QString SELECTIONS;
    //Identifier for an image.
    const static QString IMAGE;
    //Identifier for a channel.
    static const QString CHANNEL;

    static bool m_registered;

signals:
    void indexChanged( bool forceReload );

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

    //Set either the upper or lower bound for the index.
    void _setFrameBounds(/*StateKey key*/const QString& key, const QString& val);

    //Set the upper, lower, or index value, checking that it is a valid value.
    bool _setFrameBoundsCheck(/*StateKey key*/const QString& key , int bound);

    Selection( const Selection& other);
    Selection operator=( const Selection& other );
};
}
}
