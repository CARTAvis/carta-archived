/**
 * Stores the results of a 1-d curve fit.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
#include "CartaLib/Fit1DInfo.h"


namespace Carta{
namespace Lib{

namespace Hooks {

class FitResult : public Plot2DResult {
  
  public:

    FitResult();


    /**
     * Return the status of the fit (whether or not it worked).
     * @return - a status message about the fit.
     */
    Fit1DInfo::StatusType getStatus() const;


  	/**
  	 * Set how well the fit worked.
  	 * @param statusMsg - information about how well the fit worked.
  	 */
  	void setStatus( Fit1DInfo::StatusType statusType );


    virtual ~FitResult(){}

  private:
      Fit1DInfo::StatusType m_status;
};
}
}
}
