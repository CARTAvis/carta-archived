#pragma once

#include <vector>
#include "Gauss1d.h"

namespace Optimization
{
namespace Gaussian1DFitting
{
class PolynomialFitter1D
{
public:

    typedef std::pair < double, double > Pair;

    PolynomialFitter1D();

    VD
    fit( int nTerms, const std::vector < Pair > & data );
};

/*
 * Gaussian 1D fitting helper
 * This is an implementation of a single gaussian fitter as described in
 *
 * Guo, H.; , "A Simple Algorithm for Fitting a Gaussian Function [DSP Tips and Tricks]," Signal Processing Magazine, IEEE , vol.28, no.5, pp.134-137, Sept. 2011
 * doi: 10.1109/MSP.2011.941846
 * URL: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5999593&isnumber=5999554
 *
 * Todo: investigate the following paper:
 *
 * Kheirati Roonizi, Ebadollah. "A New Algorithm for Fitting a Gaussian Function Riding on the Polynomial Background."
 * Signal Processing Letters, IEEE 20.11 (2013): 1062-1065. APA
 *
 */

class GuoFit
{
public:

    enum Algorithm { Caruana, NonIterativeGuo, IterativeGuo };

    typedef std::pair < double, double > Pair;

    GuoFit( const std::vector < Pair > & data );

    void
    setAlgorithm( Algorithm which ) { m_algorithm = which; }

    /// do an iteration
    bool
    iterate();

    /// get the results
    const VD &
    getResults() const { return m_results; }

protected:

    const std::vector < Pair > & m_data;
    VD m_results;

    Algorithm m_algorithm;

    bool
    runCaruana();

    bool
    runNonIterativeGuo();

    bool
    runIterativeGuo();
};
} // namespace Gaussian1DFitting
} // namespace Optimization

