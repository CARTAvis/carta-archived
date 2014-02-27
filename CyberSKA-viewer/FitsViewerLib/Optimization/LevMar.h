#ifndef LEVMAR_H
#define LEVMAR_H

/*
 * Levenberg-Marquardt optimization using gsl
 *
 * algorithm needs following input
 *  initial parameters - which also tells us the number of parameters to optimize for
 *  number of samples
 *  function for calculating F
 *  function for calculating J (finite difference used by default) -- not implemented now
 *  function for bounding (no bounds used by default)
 *  extra parameter to pass to the functions (usually 'this' for c++)
 */

#include <vector>

namespace Optimization {

class LevMar
{

public:

    enum Status { Done, Continue, Error };

    LevMar();
    ~LevMar();

    /// user provided function for calculating F
    /// \param params will be the input paramaters
    /// \param userData is supplied verbatim
    /// \param results is a pre-allocated buffer where the function should store the F[]
    typedef void (* FFunc)( const double * params, double * results, void * userData);
//    typedef void (* JFunc)( const double * params, double * results, void * userData );
    /// user supplied constraints function
    /// \param params is the input and output
    typedef void (* ConstraintsFunc)( double * params, void * userData);

    void setStartParameters( const std::vector<double>  & p);
    void setStartParameters( size_t n, const double * ptr);

    /// set the number of samples - this is needed for the lev-mar to allocate
    /// appropriate sizes of buffers for calculations. It is also the size that is
    /// expected for user function F to obbey.
    void setNumSamples( int n);

    void setFFunc( FFunc f, void * userData = 0);
//    void setJFunc( JFunc *);
    void setConstraintsFunc( ConstraintsFunc f, void * userData = 0);

    /// call to restart the lev-mar loop
    void init();
    /// successively call until you get error or success
    Status iterate();
    /// retrieve the chi square error
    double chiSq();
    /// retrieve the solution
    const double * getSolutionRef();

protected:


    // hide the implementation details from the user
    struct Impl;
    Impl * m_impl;
    Impl & impl();

};

} // namespace Optimization

#endif // LEVMAR_H
