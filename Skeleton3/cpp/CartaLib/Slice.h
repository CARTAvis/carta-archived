/**
 * Classes for describing slices of arrays, which are used as input for algorithms that
 * extract views from images. The design of this was _stronly_ influenced by
 * Python/NumPy slices.
 *
 * There are two purposes of these classes:
 *
 *   - offer an intuitive syntax for describing slices
 *   - include an algorithm that applies a slice to an array (apply family)
 *
 * Note: the apply() methods could be (and probably should be) factored out, to limit
 * classes to a single purpose.
 *
 **/


#pragma once

#include "Nullable.h"
#include <QString>
#include <cstdint>
#include <vector>


/// description of a 1D slice
/// this is inspired by the python slice object [start:end:step]
///
/// Some examples:
///
/// Pyton: [8::2]
/// C++:   Slice1D().start(8).step(2)
///
/// Python: [::-1]
/// C++:    Slice1D().step(-1)
///
/// Python: [x]
/// C++:    Slice1D(x) or Slice1D().index(x)
///
/// Note that Slice1D also supports storing a single index (to indicate the desired
/// slice should be a scalar, rather than array), e.g. a[7]

class Slice1D {

public:

    /// the type used for indexing
    /// NOTE: this could be templated...
    typedef int Index;

    /// construct a slice equivalent to [:] or [::], i.e. denothing whole array
    Slice1D();

    /// construct a slice with a single index, equivalent to [i]
    /// this is only semantically different from [i:i+1], because it represents
    /// extraction of a single element (i.e. the result of such extraction should
    /// not be an array)
    Slice1D(Index ind);

    /// set start (index of first element to be extracted)
    Slice1D & start( Index s);

    /// set stop (the index past the last element to be extracted)
    Slice1D & end( Index e);

    /// set step value
    Slice1D & step( Index s);

    /// set single index
    /// note we do not want chaining here, so we return void
    void index( Index s);

    /// return true if this slice represents single index slice
    /// this is the only difference between [i:i+1] and [i]
    bool isSingleIndex() const;

    /// is the slice null (default cosntructed)
    bool isNull() const;

    /// type of result returned from the apply method
    struct ApplyResult {
        /// index of the first element to extract
        /// -1 means bad result/error
        Index start;
        /// how many elements to extract (could be 0)
        /// if count == -1, then the extraction is of a single index
        Index count;
        /// the step
        Index step;
        /// convenience accessor to compute the last element index
        Index end() const {
            if( isSingle()) return start;
            return start + step * (count - 1);
        }
        /// convenience test for error
        bool isError() const;
        /// convenience test for single value
        /// note, the result could still be an error...
        bool isSingle() const;
        /// return a human readable string
        QString toStr() const;

        /// combine two applied results into one
        static ApplyResult combine( const ApplyResult & r1, const ApplyResult & r2);

    };

    /// return starting index, count and step for the given input size n
    /// \todo redo this so that it can be marked const
    ApplyResult apply( Index n);

    /// this is adaptation of cpython's code for PySlice_GetIndicesEx()
    /// https://docs.python.org/2/c-api/slice.html
    ApplyResult cpythonApply( Index n) const;

    /// formats the slice into a string
    /// mainly useful for debugging
    QString toStr( QString format = "[%1]") const;

protected:

    /// positive step case calculation, used in apply()
    void positiveCase( ApplyResult & res, Index n, Index off = 0) const;


    /// do we represent single index (true) or not
    bool m_singleIndex;

    /// if not single index, these are the start/end/step values
    /// if single index, then start is the index
    Nullable < Index > m_start, m_end, m_step;

};

/// contains representation of an n-dimensional slice, which is a structure
/// used to create a view of an array
///
/// this is inspired by the NumPy basic slicing strategy
///
/// Some examples of usage:
///
/// NumPy: [3::-1,:,3]
/// C++  : SliceND s;
///        s.start(3).step(-1)
///        .next()
///        .next().index(3);
///    or
///        SliceND s;
///        s.axis(0).start(3).step(-1);
///        s.axis(2).index(3);
///    or
///        typedef Slice1D S;
///        SliceND s({ S().start(3).step(-1), S(), S(3) });
///
class SliceND {

public:

    typedef Slice1D::Index Index;

    /// number of explicitly defined dimensions
    int dims() const;

    /// default constructor - creates a [:] slice (ie. everything)
    SliceND();

    /// constructor with initializer list
    SliceND( std::initializer_list<Slice1D> list);

    /// get slice at position pos
    /// if such slice is not recorded, it is created (i.e. the internal
    /// array of slices is resized)
    Slice1D & slice( size_t pos);

    /// forward 1d slice start
    SliceND & start( Index start);

    /// forward 1d slice end
    SliceND & end( Index end);

    /// forward 1d slice step
    SliceND & step( Index step);

    /// forward 1d slice index
    SliceND & index( Index index);

    /// moves to the next index (same as what colon does)
    SliceND & next();

    /// the result of applying an n-dimensional slice to an array
    /// default constructor makes an empty result, indicating error
    class ApplyResult
    {

    public:

        /// was there an error
        bool isError() const;
        /// does this result represent a single (usual scalar) value?
        /// i.e. are all 1D slices also single...
        bool isSingle() const;
        /// return the list of 1D results for each input dimension
        const std::vector<Slice1D::ApplyResult> & dims() const;
        /// make a readable string
        QString toStr() const;
        /// combine two applied results to create a new one
        static ApplyResult combine( const ApplyResult & r1, const ApplyResult & r2);

    protected:

        std::vector<Slice1D::ApplyResult> m_results;
        bool m_error = true;
        bool m_single = false;

        friend class SliceND;
    };

    /// figure out which elements to extract for the given dimensions
    /// if the numpber of supplied dimensions is greater than number of slices,
    /// the slices are padded with defaults (ie. ':'). If the number of supplied
    /// dimensions is smaller than number of slices, an error result is returned.
    ApplyResult apply( const std::vector<Index> & dimensions) const;

    /// formats the slice into a string
    /// mainly useful for debugging
    QString toStr( QString format = "[%1]") const;

protected:

    /// n-dimensional slice is just a collection of 1d-slices
    std::vector<Slice1D> m_slices;

    /// for chaining notation we keep track of current position
    int m_currentSlice = 0;
};

