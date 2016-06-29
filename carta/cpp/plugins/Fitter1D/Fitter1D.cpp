
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/Fit1DHook.h"
#include "Gaussian1dFitService.h"
#include "Fitter1D.h"
using namespace std;



#include <QDebug>


Fitter1D::Fitter1D(QObject *parent) :
    QObject(parent){

    m_fitter = new Gaussian1dFitService::Manager(this);

    connect ( m_fitter, SIGNAL(done(Gaussian1dFitService::ResultsG1dFit )),
                this, SLOT(_fitResultsCB(Gaussian1dFitService::ResultsG1dFit)) );

}



std::vector<HookId> Fitter1D::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::Fit1DHook::staticId
    };
}

Carta::Lib::Fit1DInfo::StatusType Fitter1D::_getStatus( Gaussian1dFitService::ResultsG1dFit res ) const {
    Carta::Lib::Fit1DInfo::StatusType statusType = Carta::Lib::Fit1DInfo::StatusType::OTHER;
    Gaussian1dFitService::ResultsG1dFit::Status status = res.status();
    if ( status == res.Complete ) {
        statusType = Carta::Lib::Fit1DInfo::StatusType::COMPLETE;
    }
    else if ( status == res.Partial ) {
        statusType = Carta::Lib::Fit1DInfo::StatusType::PARTIAL;
    }
    else if ( status == res.Empty ){
        statusType = Carta::Lib::Fit1DInfo::StatusType::NOT_DONE;
    }
    else {
        statusType = Carta::Lib::Fit1DInfo::StatusType::ERROR;
    }
    return statusType;
}


void Fitter1D::_fitCurves( const Carta::Lib::Fit1DInfo& info ){
    //Package the fit info up the way the Gaussian Service wants it.
    Gaussian1dFitService::InputParametersG1dFit inputParams;
    inputParams.nGaussians = info.getGaussCount();
    inputParams.poly = info.getPolyDegree();
    inputParams.data = info.getData();
    inputParams.isNull = false;
    inputParams.left = 0;
    inputParams.right = inputParams.data.size() - 1;
    inputParams.randomHeuristicsEnabled = info.isRandomHeuristics();
    std::vector<std::tuple<double,double,double> > initGuesses = info.getInitialGaussianGuesses();
    int guessCount = initGuesses.size();
    if ( guessCount > 0 ){
        int gaussParamSize = guessCount * 3;
        inputParams.initGuess = std::vector<double>( gaussParamSize + inputParams.poly, 0.0 );
        for ( int i = 0; i < guessCount; i++ ){
            inputParams.initGuess[3*i] = std::get<0>( initGuesses[i] );
            inputParams.initGuess[3*i+1] = std::get<1>( initGuesses[i] );
            inputParams.initGuess[3*i+2] = std::get<2>( initGuesses[i] );
        }
    }
    inputParams.stamp = m_lastGfId;
    m_lastGfId++;
    m_fitter->request( inputParams );
}


void Fitter1D::_fitResultsCB(Gaussian1dFitService::ResultsG1dFit res){
    Carta::Lib::Fit1DInfo::StatusType statusType = _getStatus( res );
    if ( statusType == Carta::Lib::Fit1DInfo::StatusType::COMPLETE ||
            statusType == Carta::Lib::Fit1DInfo::StatusType::PARTIAL ){
        int inputSize = res.input.data.size();
        int outputSize = inputSize * 2;
        std::vector< std::pair<double,double> > fits( outputSize );
        for ( int i = 0 ; i < outputSize ; ++i ) {
            double x = double (i) / outputSize * inputSize;
            double y = Optimization::evalNGauss1dBkg(
                    x,
                    res.input.nGaussians,
                    res.input.poly,
                    res.params );
            fits[i].first = x;
            fits[i].second = y;
        }
        m_fitResult.setData( fits );
    }
    m_fitResult.setStatus( statusType );
    m_fitResult.setDiffSquare( res.diffSq );
    m_fitResult.setRMS( res.rms );
    std::vector<double> params = res.params;
    std::vector<tuple<double,double,double> > gaussFits( res.input.nGaussians );
    std::vector<double> polyTerms( res.input.poly );
    for ( int i = 0; i < res.input.nGaussians; i++ ){
        std::tuple<double,double,double> fit( res.params[3*i], res.params[3*i+1], res.params[3*i+2]);
        gaussFits[i] = fit;
    }
    int gaussEnd = res.input.nGaussians * 3;
    for ( int i = 0; i < res.input.poly; i++ ){
        polyTerms[i] = res.params[gaussEnd + i];
    }
    m_fitResult.setPolyCoefficients( polyTerms );
    m_fitResult.setGaussFits( gaussFits );
    m_fitPromise.set_value( m_fitResult );
}


bool Fitter1D::handleHook(BaseHook & hookData){
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        return true;
    }
    else if ( hookData.is<Carta::Lib::Hooks::Fit1DHook>()){
        Carta::Lib::Hooks::Fit1DHook & hook
            = static_cast<Carta::Lib::Hooks::Fit1DHook &>( hookData);

        Carta::Lib::Fit1DInfo info  = hook.paramsPtr->m_fitInfo;
        m_fitPromise = std::promise<Carta::Lib::Hooks::FitResult>();

        //Prepare the result for the next calculation
        m_fitResult.setName( info.getId() );
        m_fitResult.setStatus( Carta::Lib::Fit1DInfo::StatusType::NOT_DONE );

        std::future<Carta::Lib::Hooks::FitResult> futureResult = m_fitPromise.get_future();
         _fitCurves( info );
         hook.result = futureResult.get();
        return true;
    }
    qWarning() << "Sorry, Fitter1D doesn't know how to handle this hook";
    return false;
}


Fitter1D::~Fitter1D(){
    delete m_fitter;
}
