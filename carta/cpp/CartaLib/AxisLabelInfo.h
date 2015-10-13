#pragma once

#include "CartaLib/HtmlString.h"

namespace Carta
{
namespace Lib
{
/// How to label an axis in an image
class AxisLabelInfo
{
public:

    enum class Formats
    {
        NONE,
        DEG_MIN_SEC,
        DECIMAL_DEG,
        HR_MIN_SEC,
        OTHER
    };

    enum class Locations {
        NORTH,
        EAST,
        SOUTH,
        WEST,
        OTHER
    };

    /**
     * Constructor.
     */
    AxisLabelInfo();

    /**
     * Destructor.
     */
    virtual ~AxisLabelInfo();

    /**
     * Return the precision for the label.
     * @return - the number of decimal places shown in the label.
     */
    int getPrecision() const;

    /**
     * Set the number of decimal places to use in the label.
     * @param decimals - the number of decimal places shown on the label.
     */
    void setPrecision( int decimals );

    /**
     * Return the label format.
     * @return - the enumerated label format.
     */
    AxisLabelInfo::Formats getFormat() const;

    /**
     * Set an enumerated label format.
     * @param format - an enumerated label format.
     */
    void setFormat( AxisLabelInfo::Formats format );

    /**
     * Return the location of the label.
     * @return - the side of the display where the label should
     *      be shown.
     */
    AxisLabelInfo::Locations getLocation() const;

    /**
     * Set the location of the label.
     * @param location - set the side of the display where the label
     *      should be shown.
     */
    void setLocation( AxisLabelInfo::Locations location );

    /**
     * Equality operator.
     * @param other an AxisLabelInfo to compare this one to.
     * @return true if the AxisLabelInfos are the same; false otherwise.
     */
    bool operator==( const AxisLabelInfo& other ) const;

    /**
     * Inequality operator.
     * @param other an AxisLabelInfo to compare this one to.
     * @return true if the AxisLabelInfos are NOT the same; false otherwise.
     */
    bool operator!=( const AxisLabelInfo& other ) const;


private:
    Formats m_format;
    Locations m_location;
    int m_precision;
};


} // namespace Lib
} // namespace Carta
