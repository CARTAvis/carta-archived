/***
 * Translator for Label formats.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/AxisLabelInfo.h"
#include <vector>


namespace Carta {

namespace Data {

class LabelFormats : public Carta::State::CartaObject {

    friend class DataGrid;

public:

    /**
     * Translates between a string representation of an axis label format and the enumerated class
     * of recognized formats.
     * @param formatStr - a string representation of an axis format.
     * @return the corresponding enumerated axis label format.
     */
    Carta::Lib::AxisLabelInfo::Formats getAxisLabelFormat( const QString& formatStr ) const;

    /**
     * Translates between a
     */
    Carta::Lib::AxisLabelInfo::Locations getAxisLabelLocation( const QString& locationStr ) const;

    /**
     * Returns the default label format.
     * @param direction - an identifier for the location of the label.
     * @return an identifier for the default label format.
     */
    QString getDefaultFormat( const QString& direction ) const;

    /**
     * Returns whether or not the format refers to THE default format or instead refers
     * to a specific format.
     * @return format - a string describing a format.
     * @return true if the passed in format specifies using the default format; false if
     *  it refers to a specific format or an unrecognized format.
     */
    bool isDefault( const QString& format ) const;

    /**
     * Returns the default format for the axis of the indicated type.
     * @param axis - the type of axis.
     * @param cs - an enumerated coordinate system type.
     * @return - the default format for the axis.
     */
    QString getDefaultFormatForAxis( Carta::Lib::AxisInfo::KnownType axis,
            const Carta::Lib::KnownSkyCS& cs ) const;

    /**
     * Translates a non case sensitive grid label side into one
     * that is case sensitive.
     * @param direction - a side of the grid that may not have the proper capitalization.
     * @return - a recognized grid side or an empty string if the grid side is not
     *      recognized.
     */
    QString getDirection( const QString& direction ) const;

    /**
     * Translates a non case sensitive label format into one
     * that is case sensitive.
     * @param format - a label format that may not have the proper capitalization.
     * @return - a recognized label format or an empty string if the label format is not
     *      recognized.
     */
    QString getFormat( const QString& format ) const;

    /**
     * Returns a list of available grid label formats.
     * @param axisInfo - an identifier for the axis.
     * @return a QStringList containing the names of available grid label formats.
     */
    QStringList getLabelFormats( Carta::Lib::AxisInfo::KnownType axisInfo ) const;

    /**
     * Returns the side of the grid opposite to the side passed in.
     * @param side - an identifier for a grid side.
     * @return - an identifier for the grid side opposite to that passed in.
     */
    QString getOppositeSide( const QString& side ) const;

    /**
     * Returns true if the label format is a valid format; false, if the format
     * is not valid.
     * @param format - a template for a grid label format.
     * @return true if the passed in format is valid; false otherwise.
     */
    bool isVisible( const QString& format ) const;

    virtual ~LabelFormats();


    const static QString CLASS_NAME;
    const static QString AXES_FORMATS;
    const static QString FORMAT;
    const static QString FORMAT_DEFAULT;
    const static QString FORMAT_NONE;
    const static QString FORMAT_DEG_MIN_SEC;
    const static QString FORMAT_DECIMAL_DEG;
    const static QString FORMAT_HR_MIN_SEC;
    const static QString LABEL_AXES;
    const static QString LABEL_FORMATS;

    const static QString EAST;
    const static QString WEST;
    const static QString NORTH;
    const static QString SOUTH;

private:
    QString _addDecimalSeconds( const QString& baseFormat, int decimals ) const;
    std::vector<int> _getFormatIndices( Carta::Lib::AxisInfo::KnownType axisIndex ) const;
    int _getIndex( const QString& format ) const;
    void _initializeDefaultState();

    std::vector<QString> m_labelFormats;

    static bool m_registered;

    LabelFormats( const QString& path, const QString& id );

    class Factory;


	LabelFormats( const LabelFormats& other);
	LabelFormats& operator=( const LabelFormats& other );
};

}
}
