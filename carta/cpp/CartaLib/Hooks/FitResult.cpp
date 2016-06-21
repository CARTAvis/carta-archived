#include <CartaLib/Hooks/FitResult.h>
#include <QDebug>

namespace Carta {
  namespace Lib {
    namespace Hooks {

FitResult::FitResult(){
}


Fit1DInfo::StatusType FitResult::getStatus() const {
    return m_status;
}


void FitResult::setStatus( Fit1DInfo::StatusType statusMsg ){
    m_status = statusMsg;
}

    }
  }
}
