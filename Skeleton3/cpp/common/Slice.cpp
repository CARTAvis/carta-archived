#include "Slice.h"

#include <QStringList>


/// ===========================================================================
/// Slice1D
/// ===========================================================================

Slice1D::Slice1D()
    : m_singleIndex( false)
{}

Slice1D::Slice1D(Slice1D::Index ind)
    : m_singleIndex( true)
    , m_start( ind)
{}

Slice1D &Slice1D::start(Slice1D::Index s)
{
    m_start = s;
    return * this;
}

Slice1D &Slice1D::end(Slice1D::Index e)
{
    m_end = e;
    return * this;
}

Slice1D &Slice1D::step(Slice1D::Index s)
{
    m_step = s;
    return * this;
}

void Slice1D::index(Slice1D::Index s)
{
    m_start = s;
    m_singleIndex = true;
}

bool Slice1D::isSingleIndex() const
{
    return m_singleIndex;
}

Slice1D::ApplyResult Slice1D::apply(Slice1D::Index n)
{
    ApplyResult res;

    // extract step or assign default value of "1"
    // this is the same for all cases
    res.step = m_step.isSet() ? m_step.val() : 1;

    // handle special case for single index
    if( isSingleIndex()) {
        res.start = m_start.val();
        if( res.start < 0) {
            res.start += n;
        }
        // if single index is out of bounds, report error
        if( res.start < 0 || res.start >= n) {
            res.start = -1;
        }
        res.count = -1;
        return res;
    }

    // special case for step=0, which is an error
    if( res.step == 0) {
        res.start = -1;
        res.count = 0;
        return res;
    }

    // special case for n=0, return empty result
    if( n == 0) {
        res.start = 0;
        res.count = 0;
        return res;
    }

    // handle positive step
    if( res.step > 0) {
        positiveCase( res, n);
    }
    // handle case where step is negative
    else {
        std::swap( m_start, m_end);
        res.step = - res.step;
        positiveCase( res, n, 1);
        std::swap( m_start, m_end);
        res.start += res.count -1;
        res.step = - res.step;
    }
    // we are done
    return res;
}

Slice1D::ApplyResult Slice1D::cpythonApply(Slice1D::Index n)
{
    ApplyResult res;

    // extract step or assign default value of "1"
    // this is the same for all cases
    res.step = m_step.isSet() ? m_step.val() : 1;

    // handle special case for single index
    if( isSingleIndex()) {
        res.start = m_start.val();
        if( res.start < 0) {
            res.start += n;
        }
        // if single index is out of bounds, report error
        if( res.start < 0 || res.start >= n) {
            res.start = -1;
        }
        res.count = -1;
        return res;
    }

    // special case for step=0, which is an error
    if( res.step == 0) {
        res.start = -1;
        res.count = 0;
        return res;
    }

    Index defstart = res.step < 0 ? n-1 : 0;
    Index defstop = res.step < 0 ? -1 : n;

    if( m_start.isNull()) {
        res.start = defstart;
    }
    else {
        res.start = m_start.val();
        if (res.start < 0) res.start += n;
        if (res.start < 0) res.start = (res.step < 0) ? -1 : 0;
        if (res.start >= n)
            res.start = (res.step < 0) ? n - 1 : n;
    }

    Index stop;
    if( m_end.isNull()) {
        stop = defstop;
    }
    else {
        stop = m_end.val();
        if (stop < 0) stop += n;
        if (stop < 0) stop = (res.step < 0) ? -1 : 0;
        if (stop >= n)
            stop = (res.step < 0) ? n - 1 : n;
    }

    if ((res.step < 0 && stop >= res.start)
        || (res.step > 0 && res.start >= stop)) {
        res.count = 0;
    }
    else if (res.step < 0) {
        res.count = (stop-res.start+1)/(res.step)+1;
    }
    else {
        res.count = (stop-res.start-1)/(res.step)+1;
    }

    return res;
}

QString Slice1D::toStr(QString format) const {
    QString res;
    if( m_start.isSet()) {
        res += QString::number( m_start.val());
    }
    if( ! isSingleIndex()) {
        res += ":";
        if( m_end.isSet()) {
            res += QString::number( m_end.val());
        }
        if( m_step.isSet()) {
            res += ":" + QString::number( m_step.val());
        }
    }
    return format.arg(res);
}

void Slice1D::positiveCase(Slice1D::ApplyResult & res, Slice1D::Index n, Slice1D::Index off)
{
    // assuming we already have step>0, n>0 and not single index, and
    // also res.step has already been extracted

    // extract start value if defined, and clamp it to be valid
    if( m_start.isSet()) {
        res.start = m_start.val();
        // adjust negative values
        if( res.start < 0) res.start = n + res.start;
        res.start += off;
        // make sure 0 <= start <= n
        res.start = clamp<Index>( res.start, 0, n);
    }
    // if start was not defined, assign a default
    else {
        res.start = 0;
    }
    // figure out the number of elements
    Index end;
    if( m_end.isSet()) {
        end = m_end.val();
        if( end < 0) end = n + end;
        end += off;
        // make sure start <= end <= n
        end = clamp<Index>( end, res.start, n);
    }
    // if end was not defined, assign default
    else {
        end = n;
    }
    // at this point we have start and end such that
    //      0 <= start <= end <= n
    // to get the count, we do a simple integer division...
    res.count = (end - res.start + res.step - 1) / res.step;
}

/// ===========================================================================
/// Slice1D::ApplyResult
/// ===========================================================================


bool Slice1D::ApplyResult::isError() const
{
    return start < 0;
}

bool Slice1D::ApplyResult::isSingle() const
{
    return count == -1;
}

QString Slice1D::ApplyResult::toStr() const
{
    if( isError()) {
        return "error";
    }
    if( isSingle()) {
        return QString("%1").arg(start);
    }
    return QString("%1+%2x%3").arg(start).arg(step).arg(count);
}


/// ===========================================================================
/// SliceND
/// ===========================================================================

SliceND::SliceND()
{
    slice( 0);
}

SliceND::SliceND(std::initializer_list<Slice1D> list)
{
    m_slices = list;
    slice( 0);
}

Slice1D & SliceND::slice(size_t pos) {
    if( size_t(pos) >= m_slices.size()) {
        m_slices.resize( pos + 1);
    }
    return m_slices[ pos];
}

SliceND & SliceND::start(SliceND::Index start) {
    slice( m_currentSlice).start( start);
    return * this;
}

SliceND & SliceND::end(SliceND::Index end) {
    slice( m_currentSlice).end( end);
    return * this;
}

SliceND & SliceND::step(SliceND::Index step) {
    slice( m_currentSlice).step( step);
    return * this;
}

SliceND & SliceND::index(SliceND::Index index) {
    slice( m_currentSlice).index( index);
    return * this;
}

SliceND & SliceND::next() {
    m_currentSlice ++;
    slice( m_currentSlice);
    return * this;
}

SliceND::ApplyResult SliceND::apply(const std::vector<SliceND::Index> & dimensions)
{
    ApplyResult res;

    // if we the number of input dimensions is less than number of slices
    // we report an error
    if( dimensions.size() < m_slices.size()) {
        return res;
    }

    res.m_single = true;
    res.m_error = false;
    for( size_t i = 0 ; i < dimensions.size() ; i ++) {
        // appy slice[i] to dimension[i]
        Slice1D::ApplyResult ar = slice(i).apply( dimensions[i]);
        // add this to the results
        res.m_results.push_back( ar);
        // update error
        res.m_error = res.m_error || ar.isError();
        // update single status
        res.m_single = res.m_single && ar.isSingle();
    }
    return res;
}

QString SliceND::toStr(QString format) const
{
    QString res;
    for( auto  & slice : m_slices) {
        if( ! res.isEmpty()) res.append( ',');
        res.append( slice.toStr( "%1"));
    }
    return format.arg( res);
}

/// ===========================================================================
/// SliceND::ApplyResult
/// ===========================================================================


bool SliceND::ApplyResult::isError() const
{
    return m_error;
}

bool SliceND::ApplyResult::isSingle() const
{
    return m_single;
}

const std::vector<Slice1D::ApplyResult> & SliceND::ApplyResult::dims() const {
    return m_results;
}

QString SliceND::ApplyResult::toStr() const
{
    QString res;
    if( isError()) {
        res += "error";
    }
    if (isSingle()) {
        res += "single";
    }

    QStringList lst, lst2;
    for( auto & s : m_results) {
        lst << s.toStr();
        lst2 << QString::number( s.count);
    }
    return res + "{" + lst.join( ',') + "} dims=" + lst2.join("x");
}
