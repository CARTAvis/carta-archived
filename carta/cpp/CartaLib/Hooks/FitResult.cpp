#include <CartaLib/Hooks/FitResult.h>
#include <QDebug>

namespace Carta {
  namespace Lib {
    namespace Hooks {

    FitResult::FitResult(){
    }

    double FitResult::getDiffSquare() const {
        return m_diffSq;
    }

    std::vector< std::tuple<double,double,double> > FitResult::getGaussFits() const {
        return m_gaussFits;
    }
    std::vector<double> FitResult::getPolyCoefficients() const {
        return m_polyCoeffs;
    }
    double FitResult::getRMS() const {
        return m_rms;
    }

    Fit1DInfo::StatusType FitResult::getStatus() const {
        return m_status;
    }

    void FitResult::setDiffSquare( double diffSquares ){
        m_diffSq = diffSquares;
    }

    void FitResult::setGaussFits( std::vector<std::tuple<double,double,double> > gaussFits ){
        m_gaussFits = gaussFits;
    }

    void FitResult::setPolyCoefficients( const std::vector<double>& coeffs ){
        m_polyCoeffs = coeffs;
    }

    void FitResult::setRMS( double rms ){
        m_rms = rms;
    }

    void FitResult::setStatus( Fit1DInfo::StatusType statusMsg ){
        m_status = statusMsg;
    }
    }
  }
}
