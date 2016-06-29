/**
 * Stores the results of a 1-d curve fit.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
#include "CartaLib/Fit1DInfo.h"
#include <memory>

namespace Carta{
namespace Lib{

namespace Hooks {

class FitResult : public Plot2DResult {
  
  public:

    FitResult();

    /**
     * Return a list of (center,peak, fbhw) Gauss fit estimates.
     * @return - a list of (center, peak, fbhw) Gauss fit estimates.
     */
    std::vector<std::tuple<double,double,double> > getGaussFits() const;

    /**
     * Return the coefficients of the polynomial fit.
     * @return - a list of coefficients for the polynomial fit.
     */
    std::vector<double> getPolyCoefficients() const;

    /**
     * Return the root mean square error of the fit.
     * @return - the root mean square error of the fit.
     */
    double getRMS() const;

    /**
     * Return the status of the fit (whether or not it worked).
     * @return - a status message about the fit.
     */
    Fit1DInfo::StatusType getStatus() const;

    /**
     * Return the sum of squares error of the fit.
     * @return - the sum of squares error of the fit.
     */
    double getDiffSquare() const;

    /**
     * Set the (center,peak,fbhw) of the Gauss fit estimates.
     * @param gaussFits - the (center,peak,fbhw) of the Gauss fit estimates.
     */
    void setGaussFits( std::vector<std::tuple<double,double,double> > gaussFits );

    /**
     * Set the coefficients of the polynomial fit.
     * @param coeffs - the coefficients of the polynomial fit.
     */
    void setPolyCoefficients( const std::vector<double>& coeffs );

    /**
     * Set the root mean square error of the fit.
     * @param rms - the root mean square error of the fit.
     */
    void setRMS( double rms );

    /**
     * Set how well the fit worked.
     * @param statusMsg - information about how well the fit worked.
     */
    void setStatus( Fit1DInfo::StatusType statusType );

    /**
     * Set the sum of squares error of the fit.
     * @param diffSquares - the sum of squares error of the fit.
     */
    void setDiffSquare( double diffSquares );

    virtual ~FitResult(){}

  private:
      Fit1DInfo::StatusType m_status;
      std::vector<double> m_polyCoeffs;
      std::vector< std::tuple<double,double,double> > m_gaussFits;
      double m_diffSq;
      double m_rms;
};
}
}
}
