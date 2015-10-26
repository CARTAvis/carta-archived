/***
 * Information about how an axis should be displayed.
 */

#pragma once

#include <vector>
#include "CartaLib/AxisInfo.h"

namespace Carta {

namespace Lib {

class AxisDisplayInfo {

public:

    /**
     * Constructor.
     */
    AxisDisplayInfo( );

    /**
     * Copy constructor.
     */
    AxisDisplayInfo( const AxisDisplayInfo& other);

    /**
     * Assignment operator.
     */
    AxisDisplayInfo& operator=( const AxisDisplayInfo& other );

    /**
     * Equality operator.
     * @param other an AxisDisplayInfo to compare this one to.
     * @return true if the AxisDisplayInfos are the same; false otherwise.
     */
    bool operator==( const AxisDisplayInfo& other ) const;

    /**
     * Inequality operator.
     * @param other an AxisDisplayInfo to compare this one to.
     * @return true if the AxisDisplayInfos are NOT the same; false otherwise.
     */
    bool operator!=( const AxisDisplayInfo& other ) const;

    /**
     * Return the axis type.
     * @return - the axis type.
     */
    Carta::Lib::AxisInfo::KnownType getAxisType() const;

    /**
     * Return the current axis frame.
     * @return - the current axis frame.
     */
    int getFrame() const;

    /**
     * Return the total number of frames in the axis.
     * @return - the total number of frames for the axis.
     */
    int getFrameCount() const;

    /**
     * Return the index of the axis in display order.
     * @return - the display index of the axis.
     */
    //For example, the spectral axis might have index 3 in the image, but
    //we would permute it to being axis 0 if we wanted to plot Channel vs Something.
    //The axes are permuted so the horizontal and vertical display axes come first.
    int getPermuteIndex() const;

    /**
     * Set the type of the axis.
     * @param type - the axis type.
     */
    void setAxisType( const Carta::Lib::AxisInfo::KnownType type );

    /**
     * Set the total number of frames in the axis.
     * @param count - the total number of frames in the axis.
     */
    void setFrameCount( int count );

    /**
     * Set the current axis frame.
     * @param frame - the current axis frame.
     */
    void setFrame( int frame );

    /**
     * Set the index of the axis in display order (the horizontal and vertical
     * display axes should be the first axes in display order).
     * @param - index of the axis in display order.
     */
    //Note: 0-based index.
    void setPermuteIndex( int index );

    /**
     * Return a string representation of the axis display information.
     * @return - a string representation of the axis display information.
     */
    QString toString() const;

    /**
     * Return true if the display axes are in the celestial plane; false if they
     * include an axis outside of the celestial plane such as a spectral axis.
     * @return - true if both display axes are in the celestial plane; false, otherwise.
     */
    static bool isCelestialPlane( const std::vector<AxisDisplayInfo>& displayInfos );

    /**
     * Return true if the display axes have been permuted from their original ordering in the
     * image; false otherwise.
     * @return - true if the display axes have been permuted; false, otherwise.
     */
    static bool isPermuted( const std::vector<AxisDisplayInfo>& displayInfos);

    /**
     * Destructor.
     */
    virtual ~AxisDisplayInfo();

private:

    Carta::Lib::AxisInfo::KnownType m_type;
    int m_frameCount;
    int m_frame;
    int m_permuteIndex;

};

}
}
