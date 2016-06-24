/**
 * A thread that computes fits for a list of curves.
 **/

#pragma once

#include "CartaLib/Hooks/FitResult.h"
#include <QThread>
#include <vector>


namespace Carta{
namespace Data{

class ProfileFitThread : public QThread {

    Q_OBJECT;

public:

    /**
     * Constructor.
     */
    ProfileFitThread( QObject* parent = nullptr );

    /**
     * Returns the fit results.
     * @return - a list of computed fit results.
     */
    std::vector<Carta::Lib::Hooks::FitResult> getResult() const;


    /**
     * Compute the fit.
     */
    void run();

    /**
     * Set the data and parameters to use for the fit.
     * @param fitInfo - a list of data with associated fit parameters.
     */
    void setParams( const std::vector<Carta::Lib::Fit1DInfo>& fitInfos  );

    /**
     * Destructor.
     */
    virtual ~ProfileFitThread();

private:
    std::vector<Carta::Lib::Fit1DInfo> m_fitInfos;
    std::vector<Carta::Lib::Hooks::FitResult> m_results;

    ProfileFitThread( const ProfileFitThread& other);
    ProfileFitThread& operator=( const ProfileFitThread& other );
};
}
}
