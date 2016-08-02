#include "LevMar.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace Optimization
{
/// Implementation using GSL's multifit algorithm (which is using levmar)
struct LevMar::Impl {
protected:

    Impl()
    {
        iter = 0;
        gslSolver = 0;
        userFFunc = 0;
        userFFuncData = 0;
        userConstrainsFunc = 0;
        userConstraintsFuncData = 0;
        ns = 0;
        f1 = f2 = xd = 0;
    };
    ~Impl()
    {
        iter = 0;
        if ( gslSolver ) { gsl_multifit_fdfsolver_free( gslSolver ); }
        if ( f1 ) { gsl_vector_free( f1 ); }
        if ( f2 ) { gsl_vector_free( f2 ); }
        if ( xd ) { gsl_vector_free( xd ); }
    };

    friend class LevMar;

    /// gsl callback for calculating F only
    /// calls user specified function with user specified data
    static int
    gslCBcalcF( const gsl_vector * x,
                void * data,
                gsl_vector * f );

    /// gsl callback for calculating J only
    static int
    gslCBcalcJ( const gsl_vector * x,
                void * data,
                gsl_matrix * J );

    /// gsl callback for calculating J and F simulatenously
    static int
    gslCBcalcJF( const gsl_vector * x,
                 void * data,
                 gsl_vector * f,
                 gsl_matrix * J );

    /// get the chi^2 for the current solution
    double
    chiSq();

    // default implementation of calculating J
    int
    calcJ( const gsl_vector * x, gsl_matrix * J );

    LevMar::FFunc userFFunc;
    void * userFFuncData;

//    JFunc * m_jFunc; void * m_jFuncData;
    LevMar::ConstraintsFunc userConstrainsFunc;
    void * userConstraintsFuncData;

    /// return the current solution
    const double *
    getSolutionRef();

    /// number of samples (size of F)
    int ns;

    /// initial parameters
    std::vector < double > initialParameters;

    /// solution buffer
    std::vector < double > solutionBuffer;

    /// iterate function
    int
    iterate();

    /// reset function
    void
    init();

    /// current iteration
    size_t iter;

    gsl_multifit_fdfsolver * gslSolver;
    gsl_multifit_function_fdf fdf;
    gsl_vector * f1, * f2, * xd; // used in calcJ

    // current solution
//    gsl_vector * x;
};

int
LevMar::Impl::gslCBcalcF( const gsl_vector * x, void * data, gsl_vector * f )
{
    LevMar::Impl & t = * static_cast < LevMar::Impl * > ( data );
    t.userFFunc( x->data, f->data, t.userFFuncData );
    return GSL_SUCCESS;
}

int
LevMar::Impl::gslCBcalcJ( const gsl_vector * x, void * data, gsl_matrix * J )
{
    LevMar::Impl & t = * static_cast < LevMar::Impl * > ( data );
    return t.calcJ( x, J );
}

int
LevMar::Impl::gslCBcalcJF( const gsl_vector * x, void * data, gsl_vector * f, gsl_matrix * J )
{
    gslCBcalcF( x, data, f );
    gslCBcalcJ( x, data, J );

    return GSL_SUCCESS;
}

double
LevMar::Impl::chiSq()
{
    const double * x = getSolutionRef();
    std::vector < double > f( ns );
    userFFunc( x, & f[0], userFFuncData );
    double res = 0;
    for ( int i = 0 ; i < ns ; i++ ) {
        res += f[i] * f[i];
    }
    return res;
}

int
LevMar::Impl::calcJ( const gsl_vector * x, gsl_matrix * J )
{
    double diff = 1e-9;

    userFFunc( x->data, f1->data, userFFuncData );

    // calculate f2 = the value of f at x+dx_i
    for ( size_t i = 0 ; i < xd-> size ; i++ ) {
        // f2 = f(x + dx_i)
        gsl_vector_memcpy( xd, x );
        gsl_vector_set( xd, i, gsl_vector_get( xd, i ) + diff );
        userFFunc( xd->data, f2->data, userFFuncData );

//        expb_f( xd, data, f2);
        gsl_vector_sub( f2, f1 );
        gsl_vector_scale( f2, 1.0 / diff );

        for ( int j = 0 ; j < ns ; j++ ) {
            double f2j = gsl_vector_get( f2, j );
            gsl_matrix_set( J, j, i, f2j );
        }
    }

    return GSL_SUCCESS;
} // LevMar::Impl::calcJ

const double *
LevMar::Impl::getSolutionRef()
{
    if ( gslSolver == 0 ) {
        throw std::runtime_error( "LevMar::Impl::getSolutionRef() - no gslSolver defined" );
    }
    if ( gslSolver->x == 0 ) {
        throw std::runtime_error( "LevMar::Impl::getSolutionRef() - x = null" );
    }
    if ( gslSolver->x->data == 0 ) {
        throw std::runtime_error( "LevMar::Impl::getSolutionRef() - x.data = null" );
    }
    if ( gslSolver->x->stride != 1 ) {
        throw std::runtime_error( "LevMar::Impl::getSolutionRef() - x.stride != 1" );
    }

    return gslSolver->x->data;
}

static void
print_state( size_t iter, gsl_multifit_fdfsolver * s )
{
    return;

    printf( "iter: %3lu x = % 15.8f % 15.8f % 15.8f % 15.8f "
            "|f(x)| = %g\n",
            iter,
            gsl_vector_get( s->x, 0 ),
            gsl_vector_get( s->x, 1 ),
            gsl_vector_get( s->x, 2 ),
            gsl_vector_get( s->x, 3 ),
            gsl_blas_dnrm2( s->f ) );
}

/// returns GSL_CONTINUE if this can be continued
int
LevMar::Impl::iterate()
{
    // apply constraints
    if ( userConstrainsFunc ) {
        userConstrainsFunc( gslSolver->x->data, userConstraintsFuncData );
    }

    int status = gsl_multifit_fdfsolver_iterate( gslSolver );

    if ( status != GSL_SUCCESS ) {
        std::cerr << "aborting because: " << gsl_strerror( status ) << "\n";
        return status;
    }

    print_state( iter, gslSolver );

    // see if we got close enough
    status = gsl_multifit_test_delta(
        gslSolver->dx, gslSolver->x,
        1e-9, 1e-9 );

//    dbg(1) << "gsl_multifit_test_delta: " << gsl_strerror (status) << "\n";

    return status;
} // LevMar::Impl::iterate

void
LevMar::Impl::init()
{
    if ( userFFunc == 0 ) {
        throw std::runtime_error( "LevMar::reset(): userFFunc = 0!!!" );
    }

    if ( gslSolver ) {
        gsl_multifit_fdfsolver_free( gslSolver );
    }
    if ( f1 ) {
        gsl_vector_free( f1 );
    }
    if ( f2 ) {
        gsl_vector_free( f2 );
    }
    if ( xd ) {
        gsl_vector_free( xd );
    }

    f1 = gsl_vector_alloc( ns );
    f2 = gsl_vector_alloc( ns );
    xd = gsl_vector_alloc( initialParameters.size() );

    gslSolver = gsl_multifit_fdfsolver_alloc(
        gsl_multifit_fdfsolver_lmder, ns, initialParameters.size() );

//    dbgHere;

    fdf.f = & gslCBcalcF;
    fdf.df = & gslCBcalcJ;
    fdf.fdf = & gslCBcalcJF;

//    dbg(1) << "Setting fdf.n = " << ns << " ------------------------\n";
    fdf.n = ns;
    fdf.p = initialParameters.size();
    fdf.params = static_cast < void * > ( this );

//    dbgHere;

    gsl_vector_view x = gsl_vector_view_array(
        initialParameters.data(), fdf.p );

//    dbgHere;

    gsl_multifit_fdfsolver_set( gslSolver, & fdf, & x.vector );

//    dbgHere;
} // LevMar::Impl::init

LevMar::LevMar()
{
    m_impl = new Impl;

    impl().userFFunc = 0;
    impl().userFFuncData = 0;
    impl().userConstrainsFunc = 0;
    impl().userConstraintsFuncData = 0;
    impl().ns = 0;

//   impl().x = gsl_vector_alloc(1);
}

LevMar::~LevMar()
{
//    gsl_vector_free( impl().x);
    delete m_impl;
}

void
LevMar::setStartParameters( const std::vector < double > & p )
{
    impl().initialParameters = p;
}

void
LevMar::setStartParameters( size_t n, const double * ptr )
{
    impl().initialParameters.resize( n );
    for ( size_t i = 0 ; i < n ; i++ ) {
        impl().initialParameters[i] = ptr[i];
    }
}

void
LevMar::setNumSamples( int n )
{
    impl().ns = n;
}

void
LevMar::setFFunc( LevMar::FFunc f, void * userData )
{
    impl().userFFunc = f;
    impl().userFFuncData = userData;
}

void
LevMar::setClampFunction( ConstraintsFunc f, void * userData )
{
    impl().userConstrainsFunc = f;
    impl().userConstraintsFuncData = userData;
}

void
LevMar::init()
{
    impl().init();
}

LevMar::Status
LevMar::iterate()
{
    int status = impl().iterate();
    if ( status == GSL_CONTINUE ) {
        return Continue;
    }
    else {
        return Done;
    }

/*
    int status;
    unsigned iter = 0;
    size_t p = impl().initialParameters.size();

    gsl_matrix * covar = gsl_matrix_alloc (p, p);

    gsl_vector_view x = gsl_vector_view_array ( guess, p);

    gsl_rng_env_setup();


    print_state (iter, s);

    while
    {
        int status = impl().iterate();

    }
    while (status == GSL_CONTINUE && iter < 500);

    gsl_multifit_covar (s->J, 0.0, covar);

#define FIT(i) gsl_vector_get(s->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

    {
        double chi = gsl_blas_dnrm2(s->f);
        double dof = ns - p;
        double c = GSL_MAX_DBL(1, chi / sqrt(dof));

        printf("chisq/dof = %g\n",  pow(chi, 2.0) / dof);

        printf ("A      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
        printf ("lambda = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
        printf ("b      = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
        printf ("bkg    = %.5f +/- %.5f\n", FIT(3), c*ERR(3));
    }
#undef FIT
#undef ERR

    printf ("status = %s\n", gsl_strerror (status));

    gsl_multifit_fdfsolver_free (s);
    gsl_matrix_free (covar);

    return Error;
    */
}

double
LevMar::chiSq()
{
    return impl().chiSq();
}

const double *
LevMar::getSolutionRef()
{
    return impl().getSolutionRef();
}

LevMar::Impl &
LevMar::impl()
{
    return * m_impl;
}
} // namespace Optimization
