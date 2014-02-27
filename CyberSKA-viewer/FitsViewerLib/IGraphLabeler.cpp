#include "IGraphLabeler.h"
#include "common.h"
#include <limits>

namespace Plot2dLabelers {

BasicLabeler::BasicLabeler()
{
    // assign some 'sensible' defaults
    m_minD = std::numeric_limits<double>::epsilon() * 100;
    m_maxD = std::numeric_limits<double>::max() / 100;
    m_min = 0.0;
    m_max = 1.0;
    m_npix = 100;
    m_dual = false;
    m_minLabelGap = -1;
    m_offset = 0.0;
}

QString BasicLabeler::format(double val)
{
    return doFormat( val + offset());
}

void BasicLabeler::setZoom(double min, double max)
{
    m_min = min + m_offset;
    m_max = max + m_offset;
    zoomWasSet();
}

void BasicLabeler::setPixels(double npix)
{
    m_npix = npix;
}

std::vector<LabelEntry> BasicLabeler::compute(TextMeasureFunc mf)
{
//    dbg(1) << "v2-compute running";

    // transform min/max
//    double min = m_valtxf( m_min);
//    double max = m_valtxf( m_max);
//    double min = m_min + offset();
//    double max = m_max + offset();

    double min = m_min;
    double max = m_max;

    // make our own copies of original delta constraints so that we can modify them
    double minD = m_minD;
    double maxD = m_maxD;

    std::vector<LabelEntry> res;

    // if parameters are non-sensical return empty list
    if( min >= max) return res;
    if( ! std::isfinite( min)) return res;
    if( ! std::isfinite( max)) return res;
    if( ! std::isfinite( minD)) return res;
    if( ! std::isfinite( maxD)) return res;
    if( m_npix < 2 || ! std::isfinite( m_npix)) return res;

    // call custom init function in case it was overloaded
    computeInit();

    // make sure maxD is sensible
    if( maxD <=  0 || std::isfinite( maxD)) {
        maxD = (max - min) * 10;
    }

    // maxN = maximum number of labels (over-estimate)
    int maxN = std::max( m_npix, 1.0);

    // calculate min label gap if negative
    double minLabelGap = m_minLabelGap;
    if( minLabelGap < 0) {
        minLabelGap = mf( "W") * ( - minLabelGap);
    }

    // calculate the initial delta
    double delta = (max - min) / maxN;

    // round delta to the nearest lower power of 10
    int deltaLog = std::floor( std::log10( delta));
    delta = std::pow( 10.0, deltaLog);

    // make sure delta is sanitized
    if( ! std::isfinite(delta)) {
        delta = pow10( std::numeric_limits<double>::min_exponent10);
    }

    // convenience closure that moves delta to the next multiple
    int nextMultiplier = 2; // cycles between 2, 5 and 10
    auto advanceDelta = [&] () -> void {
        if( nextMultiplier == 2) {
            delta *= 2; nextMultiplier = 5;
        } else if( nextMultiplier == 5) {
            delta *= 2.5; nextMultiplier = 10;
        } else {
            delta *= 2; nextMultiplier = 2;
        }
    };

//    dbg(1) << "v2-compute making delta > mind, curr. delta = " << delta;

    // make sure delta  >= minD
    while( delta < minD) {
        advanceDelta();
//        dbg(1) << "-> delta = " << delta;
    }
//    dbg(1) << "delta now = " << delta;

//    dbg(1) << "min/max = " << min << " " << max;

    // keep trying bigger and bigger delta until success, or delta > maxD
    int tryCount = 0;
    while(1)
    {
//        dbg(1) << "v2-compute starting vtry loop #" << tryCount << " with delta = "
//               << QString::number( delta, 'f', 20);

        // start with empty list
        res.clear();

        // this is a silly safety mechanism to prevent infinite loops...
        // it should not be necessary if the inputs were sanitized properly
        // TODO: can we sanitize the inputs enough to remove this? do we dare?
        if( ++ tryCount > 15) {
            return res;
        }

        // generate labels with current delta, making sure there is no overlap
        // with left/right margin and the label immediately before it
        double lastEdge = -1; // right edge of the last label in pixels
        lastEdge = std::numeric_limits<double>::lowest();
        double i1 = std::floor( min / delta);
        double i2 = std::ceil( max / delta);
        LabelEntry label;
        for( double i = i1 ; i <= i2 ; i += 1) {
            // data coordinate for this grid line and label
            double centerData = i * delta;
            // pixel coordinate for this grid line and label
            double centerPix = (centerData - min) * (m_npix-1) / (max - min);
            // skip this label altogether if the pixel coordinate is not in the range
            if( centerPix < 0 || centerPix > (m_npix-1) + std::numeric_limits<double>::epsilon()) continue;
            // make the label
            label.txt1 = makeLabel1( centerData);
            label.txt2 = isDual() ? makeLabel2( centerData) : QString();
//            label = m_lmf( centerData);
            label.centerPix = centerPix;
            // measure the label
            double size1 = mf( label.txt1);
            double size2 = mf( label.txt2);
            double size = std::max( size1, size2);
            // check if the label's start position overlaps the previous edge
            double startPos = label.centerPix - size/2;
//            dbg(1) << "lastEdge= " << lastEdge << " startPos= " << startPos;
            if( startPos < lastEdge) {
                // there is an overlap, and we already added labels to the result,
                // we abort this delta altogether
                if( res.size() > 0) {
                    // indicate we don't have a solution yet
                    res.clear();
//                    dbg(1) << "overlap with previous labels, trying bigger delta";
                    // and break out of the loop to try different delta
                    break;
                }
                // otherwise there are no labels yet, which means the label is too big
                // and it actually overlaps the left margin of the plot. This is a
                // special case, and we just discard this label. It basically means we
                // started with 'i' too little.
                else {
//                    dbg(1) << "ignoring label - too small";
                    continue;
                }
            }
            // make sure we don't go past the end of allowed pixels
            double endPos = label.centerPix + size/2;
            if( endPos > m_npix + 10) {
                // we've gone too far, break out right here with the current result
                // because any future labels should not be added anyways
//                dbg(1) << "label overlaps the end boundary, no more labels then";
                break;
            }

            // at ths point we know the label is acceptable
            // add it to the result
            res.push_back( label);
//            dbg(1) << "adding label";
            // adjust the last edge to the end position, plus some small margin (2)
            lastEdge = label.centerPix + size / 2 + minLabelGap;
        }

        // if the result contains any labels, we report this as the solution
        if( res.size() > 0) {
//            dbg(1) << "Computed " << res.size() << " labels in " << tryCount << " tries";
//            for( LabelEntry & label : res) {
//                dbg(1) << label.txt1 << " @ " << label.centerPix;
//            }
            return res;
        }

        // otherwise result is empty, so let's try bigger delta
        advanceDelta();
//        dbg(1) << "delta = " << delta;

        // if delta is too big, give up and return empty list
        if( delta > maxD) {
//            dbg(1) << "delta too big, aborting";
            return res;
        }

    } // while(1)
}

bool BasicLabeler::isDual()
{
    return m_dual;
}

void BasicLabeler::setDual(bool flag)
{
    m_dual = flag;
}

QString BasicLabeler::caption1()
{
    return doGetCaption1();
}

QString BasicLabeler::caption2()
{
    return doGetCaption2();
}

double BasicLabeler::offset() const
{
    return m_offset;
}

void BasicLabeler::setOffset(double val)
{
    m_offset = val;
}

double BasicLabeler::minDelta() const
{
    return m_minD;
}

void BasicLabeler::setMinDelta(double minD)
{
    m_minD = minD;
}
double BasicLabeler::minLabelGap() const
{
    return m_minLabelGap;
}

void BasicLabeler::computeInit()
{
    /* default implementation is empty */
}

QString BasicLabeler::makeLabel1(double x)
{
    return QString::number(x);
}

QString BasicLabeler::makeLabel2(double x)
{
    return QString::number(x);
}

QString BasicLabeler::doFormat(double val)
{
    QString res = makeLabel1( val);
    if( m_dual) {
        res += " or " + makeLabel2( val);
    }
    return res;
}

void BasicLabeler::zoomWasSet()
{
    /* default implementation does nothing */
}

QString BasicLabeler::doGetCaption1()
{
    return "";
}

QString BasicLabeler::doGetCaption2()
{
    return "";
}

void BasicLabeler::setMinLabelGap(double minLabelGap)
{
    m_minLabelGap = minLabelGap;
}

// calculates and formats the distance between two values
// the result contains formatted results (second one can be ignored if not dual)
BasicLabeler::FormattedDistance BasicLabeler::distance(double val1, double val2)
{
    return doDistance( val1 + offset(), val2 + offset());
}

// default implementation - just do a simple difference
BasicLabeler::FormattedDistance BasicLabeler::doDistance(double val1, double val2)
{
    FormattedDistance res;
    double diff = fabs( val1 - val2);

    res.v1.value.plain = QString::number( diff);
    res.v1.value.html = res.v1.value.plain;
    res.v2 = res.v1;
    return res;
}


} // namespace Plot2dLabelers



