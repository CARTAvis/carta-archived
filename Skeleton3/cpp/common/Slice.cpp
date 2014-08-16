#include "Slice.h"


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

Slice1D::AppplyResult Slice1D::apply(Slice1D::Index n)
{
    AppplyResult res;

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

Slice1D::AppplyResult Slice1D::cpythonApply(Slice1D::Index n)
{
    AppplyResult res;

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

void Slice1D::positiveCase(Slice1D::AppplyResult & res, Slice1D::Index n, Slice1D::Index off)
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


bool Slice1D::AppplyResult::isError() const
{
    return start < 0;
}

bool Slice1D::AppplyResult::isSingle() const
{
    return count == -1;
}


/// ===========================================================================
/// SliceND
/// ===========================================================================

SliceND::SliceND() {}

SliceND::SliceND(std::initializer_list<Slice1D> list) {
    m_slices = list;
}

Slice1D & SliceND::axis(SliceND::Index pos) {
    if( size_t(pos) >= m_slices.size()) {
        m_slices.resize( pos + 1);
    }
    return m_slices[ pos];
}

SliceND & SliceND::start(SliceND::Index start) {
    axis( m_currentSlice).start( start);
    return * this;
}

SliceND & SliceND::end(SliceND::Index end) {
    axis( m_currentSlice).end( end);
    return * this;
}

SliceND & SliceND::step(SliceND::Index step) {
    axis( m_currentSlice).end( step);
    return * this;
}

SliceND & SliceND::index(SliceND::Index index) {
    axis( m_currentSlice).index( index);
    return * this;
}

SliceND & SliceND::next() {
    m_currentSlice ++;
    axis( m_currentSlice);
    return * this;
}
