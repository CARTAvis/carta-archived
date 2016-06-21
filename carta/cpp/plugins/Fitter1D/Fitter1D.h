/**
 * Produces polynomial and Gaussian fits for 1-D curves.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/Hooks/FitResult.h"

#include "Gaussian1dFitService.h"
#include <QObject>
#include <vector>
#include <future>

class Fitter1D : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    /**
     * Constructor.
     */
    Fitter1D(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual ~Fitter1D();

private slots:
    void _fitResultsCB(Gaussian1dFitService::ResultsG1dFit res);

private:
    Carta::Lib::Fit1DInfo::StatusType _getStatus( Gaussian1dFitService::ResultsG1dFit res ) const;
    void _fitCurves( const Carta::Lib::Fit1DInfo& info );

    Gaussian1dFitService::Manager* m_fitter;
    Carta::Lib::Hooks::FitResult m_fitResult;
    //Gaussian1dFitService::InputParametersG1dFit m_inputParams;
    qint64 m_lastGfId = 0;

    //Using a promise because the fit calculation was implemented asynchronously
    //whereas the plugin architecture is expecting a synchronous result.
    std::promise<Carta::Lib::Hooks::FitResult> m_fitPromise;

};
