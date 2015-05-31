/***
 * List of available channel units (for histogram).
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class ChannelUnits : public Carta::State::CartaObject {

public:

    /**
     * Converts the value from the old units to the new units;
     * @param oldUnits the original channel units.
     * @param newUnits the desired channel units.
     * @param value the value to convert.
     * @return the converted value or the original value if the conversion units were not
     *      supported.
     */
    double convert( const QString& oldUnits, const QString& newUnits, double value ) const;

    /**
     * Returns the clip value associated with the index.
     * @param index a integer representing a valid clip index.
     * @return the clip associated with the index.
     */
    QString getUnit( int index ) const;

    /**
     * Returns the index of the corresponding clip value or -1 if there is no such clip value.
     * @param clipValue a QString containing a clip amount.
     * @return the index of the corresponding clip value if one exists or -1 otherwise.
     */
    int getIndex( const QString& unit ) const;

    /**
     * Returns the default channel unit.
     * @return the default channel unit.
     */
    QString getDefaultUnit() const;

    const static QString CLASS_NAME;
    const static QString UNIT_LIST;
    virtual ~ChannelUnits();

private:
    void _initializeDefaultState();
    double _convert( int oldIndex, int newIndex, double value ) const;

    static bool m_registered;
    const static QString UNIT_COUNT;
    QStringList m_unitList;
    ChannelUnits( const QString& path, const QString& id );
    class Factory;


	ChannelUnits( const ChannelUnits& other);
	ChannelUnits operator=( const ChannelUnits& other );
};
}
}
