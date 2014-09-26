/***
 * Handles stores/retrieve state involving an index that is bounded both below and above;
 * the bounds and index are assumed to be nonnegative integers.
 */

#pragma once

#include <QString>
#include <State/StateKey.h>

class DataSelection {

public:

    /**
     * Constructor.
     * @param identifier the lookup id for the selection.
     * @param lowKey a StateKey representing the lower bound for the selection.
     * @param indexKey a StateKey representing the current index of the selection.
     * @param highKey a StateKey representing an upper bound for the selection.
     */
    DataSelection(const QString& identifier, StateKey lowKey, StateKey indexKey,
            StateKey highKey);

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
     * @param newIndex the new selection index.
     */
    void setIndex(int newIndex);

    /**
     * Sets the current index of the selection, provided it is in the selection range.
     * @param val a String representing the new selection index.
     * @return the index as an integer.
     */
    int setIndex(const QString& val);

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

    virtual ~DataSelection();

private:
    //Set initial values of the state if they do not already exist.
    void _initializeStates();

    //Set the index, first checking that it is within bounds.
    bool _setIndexCheck(int frameValue);

    //Returns the value of the passed in key as an integer.
    int _getValue(StateKey key) const;

    //Set either the upper or lower bound for the index.
    void _setFrameBounds(StateKey key, const QString& val);

    //Set either the upper or lower bound for the index, checking that it is a valid value.
    void _setFrameBoundsCheck(StateKey key, int bound);

    //Unique identifier for the selection.
    QString m_id;

    //Bounds and index for the state.
    StateKey m_highKey;
    StateKey m_indexKey;
    StateKey m_lowKey;

};
