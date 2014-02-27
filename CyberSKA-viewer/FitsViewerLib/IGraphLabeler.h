#ifndef IGRAPHLABELER_H
#define IGRAPHLABELER_H

#include <QString>
#include <QFont>
#include <functional>
#include <memory>
#include "common.h"

// ==============================================================================
// new classes
// ==============================================================================

namespace Plot2dLabelers {

/// entry for a label (with two texts)
struct LabelEntry {
    /// label text (formatted for html)
    QString txt1, txt2;
    /// center of the label in pixel coordinates
    /// this is also where the grid line should be drawn
    double centerPix;
};

/// function signature for measuring text
/// since we use std::function, a functor object is acceptable as well
typedef std::function< double( const QString & str) > TextMeasureFunc;

//template < class T >
//class Common {
//public:
//    typedef T Me;
//    typedef std::shared_ptr<T> SharedPtr;
//    typedef std::unique_ptr<T> UniquePtr;
//};

///
/// \brief Interface for labeling axes in 2d plots. This is the minimal interface
/// that algorithms that render graphs would want implemented.
///
class IAxisLabeler /*: public Common<IAxisLabeler>*/
{
public:

    typedef IAxisLabeler Me;
    typedef std::shared_ptr< Me > SharedPtr;
    typedef std::unique_ptr< Me > UniquePtr;

    /// result of distance calculation
    struct FormattedDistance {
        struct HtmlPlain { QString plain, html; };
        struct FormattedValue { HtmlPlain value, unit; };
        FormattedValue v1, v2;
    };

    /// formats the value
    /// if dual mode is enabled, both formats will be returned
    /// the formatted string may include units if applicable
    virtual QString format( double val) = 0;
    /// set zoom min/max
    /// min is the value at pixel 0, max is the value at the last pixel
    virtual void setZoom( double min, double max) = 0;
    /// set the number of pixels available for drawing
    virtual void setPixels( double npix) = 0;
    /// computes labels for the given parameters, with the given measuring function
    virtual std::vector<LabelEntry> compute(TextMeasureFunc mf) = 0;
    /// returns true if the labeler supports dual labels
    virtual bool isDual() = 0;
    /// returns caption for the first set of labels
    /// empty string means no caption
    virtual QString caption1() = 0;
    /// returns caption for the second set of labels (if dual)
    /// empty string means no caption
    virtual QString caption2() = 0;
    /// calculate distance between two values
    virtual FormattedDistance distance( double val1, double val2) = 0;
    /// virtual destructor
    /*virtual*/ ~IAxisLabeler() { /*empty*/ }
};

/**
 * @brief Basic algorithm for labeling vertical axis in 2d plots. It is an
 *   implementation of the IAxisLabeler. It allows for customization through
 *   sublassing and overriding protected virtual functions. In its basic form
 *   it has no units and no captions. It also allows for a simple constant
 *   offset that is simply added to all incoming values.
 */
class BasicLabeler : public IAxisLabeler
{

public:

    typedef BasicLabeler Me;
    typedef std::shared_ptr< Me > SharedPtr;

    /// constructor
    BasicLabeler();

    /// formats the value with the unit if applicable
    /// calls doFormat() to do the actual work, wthich can be overriden
    virtual QString format( double val) CPP11_FINAL_OVERRIDE;

    /// set zoom min/max
    /// min is the value at pixel 0, max is the value at the last pixel (npix-1)
    /// sets the m_min and m_max members to the requested values, after applying offset
    /// to them, then calls zoomWasSet(), which can be overriden
    virtual void setZoom( double min, double max) CPP11_FINAL_OVERRIDE;

    /// set the number of pixels available
    virtual void setPixels( double npix) CPP11_FINAL_OVERRIDE;

    /// computes labels for the given parameters, with the given measuring function
    /// it calls 3 customization methods during it's work: makeLabel1(), makeLabel2()
    /// and computeInit(), through which its behavior can be customized
    virtual std::vector<LabelEntry> compute(TextMeasureFunc mf) CPP11_FINAL_OVERRIDE;

    /// returns true if the labeler supports dual labels
    virtual bool isDual() CPP11_FINAL_OVERRIDE;

    /// returns the caption for first set of labels
    /// calls customizatio method doGetCaption1() to do the work, which can be overriden
    virtual QString caption1() CPP11_FINAL_OVERRIDE;

    /// returns the caption for 2nd set of labels
    /// calls customization method doGetCaption2() to do the work, which can be overriden
    virtual QString caption2() CPP11_FINAL_OVERRIDE;

    /// calculate distance between 2 values
    /// calls customization method doDistance() to do the work
    virtual FormattedDistance distance(double val1, double val2) CPP11_FINAL_OVERRIDE;

    /// sets the dual flag
    void setDual( bool flag);

    /// returns the current offset
    double offset() const;

    /// sets the offset
    void setOffset( double val);

    /// set the minimum size of the delta between two consecutive labels
    /// for example, setting this to 1.0 will only generate integers
    void setMinDelta(double minDelta);

    /// returns the minimum delta
    double minDelta() const;

    /// sets the minimum gap for labels
    /// positive value means the gap is in pixels
    /// negative value means the gap is in character size, which is obtained by measuring
    /// the letter 'W'
    /// default value is -1, i.e. the size of letter 'W'
    void setMinLabelGap(double minLabelGap);

    /// returns the minimum label gap
    double minLabelGap() const;

protected:

    /// customization method - called at the beginning of the compute() algorithm
    /// with the intended range (already transformed)
    virtual void computeInit();

    /// customization method - called to compute a label entry during compute()
    virtual QString makeLabel1( double x);

    /// customization method - called to compute a label entry during compute()
    virtual QString makeLabel2( double x);

    /// customization method - to format a value, called by format()
    /// the value passed to this method is already adjusted by offset
    /// the default implementation uses makeLabel1() and makeLabel2() [if dual]
    virtual QString doFormat( double val);

    /// customization method - called by setZoom() after m_min and m_max have been
    /// set (with offset applied)
    virtual void zoomWasSet();

    /// customization method - called by caption1() to compute the caption for first
    /// set of labels
    virtual QString doGetCaption1();

    /// customization method - called by caption2() to compute the caption for 2nd
    /// set of labels
    virtual QString doGetCaption2();

    /// customization method - called by distance() to compute the difference between
    /// two values. It is called with offset already applied.
    /// default implementation just returns absolute value of the difference
    virtual FormattedDistance doDistance(double val1, double val2);


    // data members
protected:
    bool m_dual;
    double m_min, m_max, m_npix;
    double m_minD, m_maxD;
    double m_minLabelGap;
    double m_offset;
};


}

#endif // IGRAPHLABELER_H
