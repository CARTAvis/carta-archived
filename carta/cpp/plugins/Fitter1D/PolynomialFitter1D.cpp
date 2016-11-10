#include "PolynomialFitter1D.h"

#include <gsl/gsl_multifit.h>
#include <gsl/gsl_linalg.h>
#include <sstream>

// this is the exception the gsl error trap will throw
class gsl_exception : public std::runtime_error
{
public:

    explicit gsl_exception (
        const std::string & p_reason,
        const std::string & p_file,
        int p_line,
        int p_gslErrno
        )
        : std::runtime_error( "GSL global error" )
    {
        reason = p_reason;
        file = p_file;
        line = p_line;
        gslErrno = p_gslErrno;
    }

    virtual ~gsl_exception() throw ( ) { }

    virtual const char *
    what() const throw ( )
    {
        std::ostringstream out;
        out << "GSL-error:" << file << ":" << line << ":" << reason << "["
            << gslErrno << "]";
        std::string stdstr = out.str();
        return stdstr.c_str();
    }

    std::string reason, file;
    int line, gslErrno;
};

/// global gsl error handler
static void
gslErrorHandler(
    const char * reason,
    const char * file,
    int line,
    int gsl_errno )
{
    qCritical() << "GSL-ERROR:" << reason
                << " loc: " << file << ":" << line
                << " errno: " << gsl_errno
                << " strerr: " << gsl_strerror( gsl_errno );

    throw gsl_exception( reason, file, line, gsl_errno );
}

namespace Optimization
{
namespace Gaussian1DFitting
{
PolynomialFitter1D::PolynomialFitter1D()
{ }

VD
PolynomialFitter1D::fit( int nTerms, const std::vector < PolynomialFitter1D::Pair > & data )
{
//    dbg(1) << "fit(" << nTerms << "," << data.size() << " elements)\n";

    VD res( nTerms, 0.0 );

    // special case 1: nTerms is 0
    if ( nTerms == 0 ) {
        return res;
    }

    // special case 2: no data
    int nData = int ( data.size() );
    if ( nData == 0 ) {
        return res;
    }

    // we delegate fitting to GSL
    gsl_matrix * X = gsl_matrix_alloc( nData, nTerms );
    gsl_vector * y = gsl_vector_alloc( nData );
    gsl_vector * c = gsl_vector_alloc( nTerms );
    gsl_matrix * cov = gsl_matrix_alloc( nTerms, nTerms );
    for ( int i = 0 ; i < nData ; i++ ) {
        double xi = data[i].first;
        double yi = data[i].second;

        double pow_xi_j = 1.0;
        for ( int j = 0 ; j < nTerms ; j++ ) {
            gsl_matrix_set( X, i, j, pow_xi_j );
            pow_xi_j *= xi;
        }

        gsl_vector_set( y, i, yi );
    }

    gsl_multifit_linear_workspace * work
        = gsl_multifit_linear_alloc( nData, nTerms );
    double chisq;
    gsl_multifit_linear( X, y, c, cov, & chisq, work );
    gsl_multifit_linear_free( work );

    // copy data from gsl to our result
    for ( int i = 0 ; i < nTerms ; i++ ) {
        res[i] = gsl_vector_get( c, i );
    }

//    dbg(1) << "fit done with chisq:" << chisq << "\n";

    // free up GSL memory
    gsl_matrix_free( X );
    gsl_vector_free( y );
    gsl_vector_free( c );
    gsl_matrix_free( cov );

    return res;
} // PolynomialFitter1D::fit

GuoFit::GuoFit( const std::vector < GuoFit::Pair > & data )
    : m_data( data )
    , m_results( 3, 0.0 / 0.0 )
    , m_algorithm( IterativeGuo )
{ }

bool
GuoFit::iterate()
{
    auto old_handler = gsl_set_error_handler( gslErrorHandler );

    bool res = false;

    try {
        if ( m_algorithm == Caruana ) {
            res = runCaruana();
        }
        else if ( m_algorithm == NonIterativeGuo ) {
            res = runNonIterativeGuo();
        }
        else {
            res = runIterativeGuo();
        }
    }
    catch ( const gsl_exception & e ) {
        qCritical() << "gsl exception in iterate:" << e.what();
    }
    catch ( const std::runtime_error & e ) {
        qCritical() << "runtime exception in iterate:" << e.what() << "\n";
    }
    catch ( ... ) {
        qCritical() << "exception in iterate!!!\n";
    }

    gsl_set_error_handler( old_handler );

    return res;
} // GuoFit::iterate

bool
GuoFit::runCaruana()
{
//    dbg(1) << "Running caruana\n";
    // prepare the matrix aa and the right hand side bb

    // aa = [  N        sum(x)   sum(x^2)     where N = number of positive entries
    //         sum(x)   sum(x^2) sum(x^3)
    //         sum(x^2) sum(x^3) sum(x^4) ]
    // bb = [ sum( ln(y))
    //        sum( x * ln(y))
    //        sum( x^2 * ln(y)) ]

    static const double EPS = 1e-9;

    double N = 0, sumx = 0, sumxx = 0, sumxxx = 0, sumxxxx = 0;
    double sumlny = 0, sumxlny = 0, sumxxlny = 0;
    for ( size_t i = 0 ; i < m_data.size() ; i++ ) {
        double x = m_data[i].first;
        double y = m_data[i].second;
        if ( ! std::isfinite( x ) ) {
            continue;
        }
        if ( ! ( y > EPS ) ) {
            continue;               // skip negatives & NANs
        }

        // TODO: low priority - optimize this at least a litle bit
        N++;
        sumx += x;
        sumxx += x * x;
        sumxxx += x * x * x;
        sumxxxx += x * x * x * x;
        sumlny += log( y );
        sumxlny += x * log( y );
        sumxxlny += x * x * log( y );
    }

//    dbg(1) << "calculated:\n"
//           << "N = " << N << "\n"
//           << "sumx = " << sumx << "\n"
//           << "sumxx = " << sumxx << "\n"
//           << "sumxxx = " << sumxxx << "\n"
//           << "sumxxxx = " << sumxxxx << "\n"
//           << "sumlny = " << sumlny << "\n"
//           << "sumxlny = " << sumxlny << "\n"
//           << "sumxxlny = " << sumxxlny << "\n";

    double aa[] = {
        N,    sumx,   sumxx,
        sumx, sumxx, sumxxx,
        sumxx, sumxxx, sumxxxx
    };
    double bb[] = {
        sumlny, sumxlny, sumxxlny
    };

    gsl_matrix_view m = gsl_matrix_view_array( aa, 3, 3 );
    gsl_vector_view b = gsl_vector_view_array( bb, 3 );
    gsl_vector * x = gsl_vector_alloc( 3 );

    int s;
    gsl_permutation * p = gsl_permutation_alloc( 3 );
    gsl_linalg_LU_decomp( & m.matrix, p, & s );
    gsl_linalg_LU_solve( & m.matrix, p, & b.vector, x );

    // retrieve the results
    {
        double a = gsl_vector_get( x, 0 );
        double b = gsl_vector_get( x, 1 );
        double c = gsl_vector_get( x, 2 );

//        dbg(1) << "a = " << a << " b = " << b << " c = " << c << "\n";
        // convert from coordinate change
        double mean = - b / ( 2 * c );
        double sigmaSq = - 1 / ( 2 * c );
        double ampl = exp( a - b * b / ( 4 * c ) );

        // convert to our gaussian parameters (see Gauss1d.h)
        m_results[0] = mean;
        m_results[1] = ampl;
        m_results[2] = - 1 / ( 2 * sigmaSq );
    }

    gsl_permutation_free( p );
    gsl_vector_free( x );

    return true;
} // GuoFit::runCaruana

bool
GuoFit::runNonIterativeGuo()
{
//    dbg(1) << "Running non-iterative Guo";

    // prepare the matrix aa and the right hand side bb

    // aa = [  sum(y^2)    sum(xy^2)    sum(x^2y^2)
    //         sum(xy^2)   sum(x^2y^2)  sum(x^3y^2)
    //         sum(x^2y^2) sum(x^3y^2)  sum(x^4y^2) ]
    // bb = [ sum( y^2 * ln(y))
    //        sum( x * y^2 * ln(y))
    //        sum( x^2 * y^2 * ln(y)) ]

    static const double EPS = 1e-9;

    double sumyy = 0, sumxyy = 0, sumxxyy = 0,
           sumxxxyy = 0, sumxxxxyy = 0,
           sumyylny = 0, sumxyylny = 0, sumxxyylny = 0;
    for ( size_t i = 0 ; i < m_data.size() ; i++ ) {
        double x = m_data[i].first;
        double y = m_data[i].second;
        if ( ! std::isfinite( x ) ) {
            continue;
        }
        if ( ! ( y > EPS ) ) {
            continue;              // skip negatives & NANs
        }
        double xx = x * x;
        double xxx = xx * x;
        double xxxx = xx * xx;
        double yy = y * y;
        double lny = log( y );

        sumyy += yy;
        sumxyy += x * yy;
        sumxxyy += xx * yy;
        sumxxxyy += xxx * yy;
        sumxxxxyy += xxxx * yy;
        sumyylny += yy * lny;
        sumxyylny += x * yy * lny;
        sumxxyylny += xx * yy * lny;
    }

//    dbg(1) << "calculated:\n"
//           << "sumyy = " << sumyy << "\n"
//           << "sumxyy = " << sumxyy << "\n"
//           << "sumxxyy = " << sumxxyy << "\n"
//           << "sumxxxyy = " << sumxxxyy << "\n"
//           << "sumxxxxyy = " << sumxxxxyy << "\n"
//           << "sumxyylny = " << sumxyylny << "\n"
//           << "sumxxyylny = " << sumxxyylny << "\n";

    double aa[] = {
        sumyy, sumxyy, sumxxyy,
        sumxyy, sumxxyy, sumxxxyy,
        sumxxyy, sumxxxyy, sumxxxxyy
    };
    double bb[] = {
        sumyylny, sumxyylny, sumxxyylny
    };

    gsl_matrix_view m = gsl_matrix_view_array( aa, 3, 3 );
    gsl_vector_view b = gsl_vector_view_array( bb, 3 );
    gsl_vector * x = gsl_vector_alloc( 3 );

    int s;
    gsl_permutation * p = gsl_permutation_alloc( 3 );
    gsl_linalg_LU_decomp( & m.matrix, p, & s );
    gsl_linalg_LU_solve( & m.matrix, p, & b.vector, x );

    // retrieve the results
    {
        double a = gsl_vector_get( x, 0 );
        double b = gsl_vector_get( x, 1 );
        double c = gsl_vector_get( x, 2 );

//        dbg(1) << "a = " << a << " b = " << b << " c = " << c << "\n";
        // convert from coordinate change
        double mean = - b / ( 2 * c );
        double sigmaSq = - 1 / ( 2 * c );
        double ampl = exp( a - b * b / ( 4 * c ) );

        // convert to our gaussian parameters (see Gauss1d.h)
        m_results[0] = mean;
        m_results[1] = ampl;
        m_results[2] = - 1 / ( 2 * sigmaSq );
    }

    gsl_permutation_free( p );
    gsl_vector_free( x );

    return true;
} // GuoFit::runNonIterativeGuo

bool
GuoFit::runIterativeGuo()
{
//    dbg(1) << "Running iterative Guo";

    // prepare the matrix aa and the right hand side bb

    // aa = [  sum(y^2)    sum(xy^2)    sum(x^2y^2)
    //         sum(xy^2)   sum(x^2y^2)  sum(x^3y^2)
    //         sum(x^2y^2) sum(x^3y^2)  sum(x^4y^2) ]
    //
    // bb = [ sum( y^2 * ln(y))
    //        sum( x * y^2 * ln(y))
    //        sum( x^2 * y^2 * ln(y)) ]
    //
    // where y^2 is actually y_(k-1)^2
    // where y_0 = y
    //   and y_k = exp(a_k + b_k * x + c_k * x * x)

    static const double EPS = 1e-9;
    static const double MinimumImprovement = 1e-6;
    static const int MaxIterations = 100;
    static const int MaxConsecutiveFails = 10;

    double lastDiffSq = 0.0 / 0.0;
    int nConsecutiveFails = 0;

    double a = 0 / 0.0, b = 0 / 0.0, c = 0 / 0.0;
    for ( int k = 0 ; k < MaxIterations ; k++ ) {
        double sumyy = 0, sumxyy = 0, sumxxyy = 0,
               sumxxxyy = 0, sumxxxxyy = 0,
               sumyylny = 0, sumxyylny = 0, sumxxyylny = 0;
        for ( size_t i = 0 ; i < m_data.size() ; i++ ) {
            double x = m_data[i].first;
            double y = m_data[i].second;
            if ( ! std::isfinite( x ) ) {
                continue;
            }
            if ( ! ( y > EPS ) ) {
                continue;              // skip negatives & NANs
            }
            double xx = x * x;
            double xxx = xx * x;
            double xxxx = xx * xx;
            double yy;
            if ( k == 0 ) {
                yy = y * y;
            }
            else {
                yy = exp( a + b * x + c * xx );
            }
            double lny = log( y );

            sumyy += yy;
            sumxyy += x * yy;
            sumxxyy += xx * yy;
            sumxxxyy += xxx * yy;
            sumxxxxyy += xxxx * yy;
            sumyylny += yy * lny;
            sumxyylny += x * yy * lny;
            sumxxyylny += xx * yy * lny;
        }

//        dbg(1) << "calculated:\n"
//               << "sumyy = " << sumyy << "\n"
//               << "sumxyy = " << sumxyy << "\n"
//               << "sumxxyy = " << sumxxyy << "\n"
//               << "sumxxxyy = " << sumxxxyy << "\n"
//               << "sumxxxxyy = " << sumxxxxyy << "\n"
//               << "sumxyylny = " << sumxyylny << "\n"
//               << "sumxxyylny = " << sumxxyylny << "\n";

        double aa[] = {
            sumyy, sumxyy, sumxxyy,
            sumxyy, sumxxyy, sumxxxyy,
            sumxxyy, sumxxxyy, sumxxxxyy
        };
        double bb[] = {
            sumyylny, sumxyylny, sumxxyylny
        };

        gsl_matrix_view mview = gsl_matrix_view_array( aa, 3, 3 );
        gsl_vector_view bview = gsl_vector_view_array( bb, 3 );
        gsl_vector * x = gsl_vector_alloc( 3 );

        int s;
        gsl_permutation * p = gsl_permutation_alloc( 3 );
        gsl_linalg_LU_decomp( & mview.matrix, p, & s );
        gsl_linalg_LU_solve( & mview.matrix, p, & bview.vector, x );

        // retrieve the results
        {
            a = gsl_vector_get( x, 0 );
            b = gsl_vector_get( x, 1 );
            c = gsl_vector_get( x, 2 );

//            dbg(1) << "a = " << a << " b = " << b << " c = " << c << "\n";
            // convert from coordinate change
            double mean = - b / ( 2 * c );
            double sigmaSq = - 1 / ( 2 * c );
            double ampl = exp( a - b * b / ( 4 * c ) );

            // convert to our gaussian parameters (see Gauss1d.h)
            m_results[0] = mean;
            m_results[1] = ampl;
            m_results[2] = - 1 / ( 2 * sigmaSq );
        }

        gsl_permutation_free( p );
        gsl_vector_free( x );

        // calculate diffSq
        double diffSq = 0;
        for ( size_t i = 0 ; i < m_data.size() ; i++ ) {
            double x = m_data[i].first;
            double y = m_data[i].second;
            double f = exp( a + b * x + c * x * x );
            if ( ! std::isfinite( x ) ) {
                continue;
            }
            if ( ! ( y > EPS ) ) {
                continue;              // skip negatives & NANs
            }
            diffSq += ( y - f ) * ( y - f );
        }

        // did we improve solution enough?
        bool improvement = false;
        if ( std::isnan( lastDiffSq ) ) {
            improvement = true;
        }
        else {
            if ( ( lastDiffSq - diffSq ) / lastDiffSq > MinimumImprovement ) {
                improvement = true;
            }
        }
        lastDiffSq = diffSq;
        if ( improvement ) {
            nConsecutiveFails = 0;
        }
        else {
            nConsecutiveFails++;
        }

//        dbg(1) << "guo(" << k << "): nf=" << nConsecutiveFails
//               << " diffSq = " << diffSq << "\n";

        if ( nConsecutiveFails > MaxConsecutiveFails ) {
            break;
        }
    }

    return true;
} // GuoFit::runIterativeGuo
} // namespace Gaussian1DFitting
} // namespace Optimization
