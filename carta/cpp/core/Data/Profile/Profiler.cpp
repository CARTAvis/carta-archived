#include "Profiler.h"
#include "CurveData.h"
#include "GenerateModes.h"
#include "ProfilePlotStyles.h"
#include "Data/Clips.h"
#include "Data/DataLoader.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Image/Layer.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Data/Plotter/LegendLocations.h"
#include "Data/Plotter/Plot2DManager.h"
#include "Data/Plotter/LineStyles.h"
#include "Data/Profile/Fit/ProfileFitService.h"
#include "Data/Profile/Render/ProfileRenderService.h"
#include "Data/Profile/ProfileStatistics.h"
#include "Data/Region/RegionControls.h"
#include "Data/Units/UnitsFrequency.h"
#include "Data/Units/UnitsIntensity.h"
#include "Data/Units/UnitsSpectral.h"
#include "Data/Units/UnitsWavelength.h"
#include "Plot2D/Plot2DGenerator.h"
#include "CartaLib/Hooks/Plot2DResult.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/Hooks/Fit1DHook.h"
#include "CartaLib/Hooks/ProfileHook.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/ProfileInfo.h"
#include "State/UtilState.h"
#include "Globals.h"
#include "PluginManager.h"
#include <QtCore/qmath.h>
#include <QTime>
#include <QList>
#include <QDebug>

namespace Carta {

namespace Data {

const QString Profiler::CLASS_NAME = "Profiler";
const QString Profiler::AUTO_GENERATE = "autoGenerate";
const QString Profiler::AXIS_UNITS_BOTTOM = "axisUnitsBottom";
const QString Profiler::AXIS_UNITS_LEFT = "axisUnitsLeft";
const QString Profiler::CURVES = "curves";
const QString Profiler::CURVE_SELECT = "selectCurve";
const QString Profiler::FIT_STATISTICS = "fitStats";
const QString Profiler::GAUSS_COUNT = "gaussCount";
const QString Profiler::GEN_MODE = "genMode";
const QString Profiler::GRID_LINES = "gridLines";
const QString Profiler::HEURISTICS = "heuristics";
const QString Profiler::IMAGE_SELECT = "imageSelect";
const QString Profiler::LEGEND_LOCATION = "legendLocation";
const QString Profiler::LEGEND_EXTERNAL = "legendExternal";
const QString Profiler::LEGEND_SHOW = "legendShow";
const QString Profiler::LEGEND_LINE = "legendLine";
const QString Profiler::MANUAL_GUESS = "manualGuess";
const QString Profiler::NO_REGION = "None";
const QString Profiler::PLOT_WIDTH = "plotWidth";
const QString Profiler::PLOT_HEIGHT = "plotHeight";
const QString Profiler::PLOT_LEFT = "plotLeft";
const QString Profiler::PLOT_TOP = "plotTop";
const QString Profiler::POLY_DEGREE = "polyDegree";
const QString Profiler::REGION_SELECT = "regionSelect";
const QString Profiler::REST_UNITS = "restUnits";
const QString Profiler::SHOW_FRAME = "showFrame";
const QString Profiler::SHOW_GUESSES = "showGuesses";
const QString Profiler::SHOW_MEAN_RMS = "showMeanRMS";
const QString Profiler::SHOW_PEAK_LABELS = "showPeakLabels";
const QString Profiler::SHOW_RESIDUALS = "showResiduals";
const QString Profiler::SHOW_STATISTICS = "showStats";
const QString Profiler::SHOW_CURSOR = "showCursor";
const QString Profiler::ZOOM_BUFFER = "zoomBuffer";
const QString Profiler::ZOOM_BUFFER_SIZE = "zoomBufferSize";
const QString Profiler::ZOOM_MIN = "zoomMin";
const QString Profiler::ZOOM_MAX = "zoomMax";
const QString Profiler::ZOOM_MIN_PERCENT = "zoomMinPercent";
const QString Profiler::ZOOM_MAX_PERCENT = "zoomMaxPercent";
const int Profiler::ERROR_MARGIN = 0.000001;


class Profiler::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new Profiler (path, id);
    }
};

bool Profiler::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Profiler::Factory());

UnitsSpectral* Profiler::m_spectralUnits = nullptr;
UnitsIntensity* Profiler::m_intensityUnits = nullptr;
GenerateModes* Profiler::m_generateModes = nullptr;
UnitsFrequency* Profiler::m_frequencyUnits = nullptr;
UnitsWavelength* Profiler::m_wavelengthUnits = nullptr;
ProfileStatistics* Profiler::m_stats = nullptr;
LineStyles* Profiler::m_lineStyles = nullptr;


QList<QColor> Profiler::m_curveColors = {Qt::blue, Qt::green, Qt::black, Qt::cyan,
        Qt::magenta, Qt::yellow, Qt::gray };

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::Plot2D::Plot2DGenerator;

Profiler::Profiler( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_linkImpl( new LinkableImpl( path )),
            m_preferences( nullptr),
            m_plotManager( new Plot2DManager( path, id ) ),
            m_legendLocations( nullptr),
            m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA) ),
            m_stateFit( UtilState::getLookup( path, CurveData::FIT)),
            m_stateFitStatistics( UtilState::getLookup( path, "fitStatistics")),
            m_renderService( new ProfileRenderService() ),
            m_fitService( new ProfileFitService() ){

    m_oldFrame = 0;
    m_currentFrame = 0;
    m_timerId = 0;

    m_residualPlotIndex = -1;

    qsrand(QTime::currentTime().msec());
    connect( m_renderService.get(),
            SIGNAL(profileResult(const Carta::Lib::Hooks::ProfileResult&,
                    std::shared_ptr<Layer>, std::shared_ptr<Region>, bool )),
            this,
            SLOT(_profileRendered(const Carta::Lib::Hooks::ProfileResult&,
                    std::shared_ptr<Layer>, std::shared_ptr<Region>, bool )));
    connect( m_fitService.get(),
               SIGNAL(fitResult(const std::vector<Carta::Lib::Hooks::FitResult>&)),
               this,
               SLOT(_fitFinished(const std::vector<Carta::Lib::Hooks::FitResult>&)));

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_preferences.reset( prefObj );

    LegendLocations* legObj = objMan->createObject<LegendLocations>();
    m_legendLocations.reset( legObj );

    m_plotManager->setPlotGenerator( new Plot2DGenerator() );
    m_plotManager->setTitleAxisY( "" );
    connect( m_plotManager.get(), SIGNAL(userSelection()), this, SLOT(_zoomToSelection()));
    connect( m_plotManager.get(), SIGNAL(userSelectionColor()), this, SLOT(_movieFrame()));
    connect( m_plotManager.get(), SIGNAL(cursorMove(double,double)),
            this, SLOT(_cursorUpdate(double,double)));
    connect( m_plotManager.get(), SIGNAL(plotSizeChanged()), this, SLOT(_plotSizeChanged()));


    _initializeStatics();
    _initializeDefaultState();
    _initializeCallbacks();

    _setErrorMargin();

    m_controllerLinked = false;
}


QString Profiler::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)),
                        this , SLOT(_loadProfile(Controller*)));
                connect(controller, SIGNAL(frameChanged(Controller*, Carta::Lib::AxisInfo::KnownType)),
                        this, SLOT( _updateChannel(Controller*, Carta::Lib::AxisInfo::KnownType)));
                connect(controller, SIGNAL(dataChangedRegion(Controller*)),
                        this, SLOT( _loadProfile( Controller*)));
                m_controllerLinked = true;
                _loadProfile( controller);
            }
        }
        else {
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                result = "Profiler only supports linking to a single image source.";
            }
        }
    }
    else {
        result = "Profiler only supports linking to images";
    }
    return result;
}


void Profiler::_assignColor( std::shared_ptr<CurveData> curveData ){
    //First go through list of fixed colors & see if there is one available.
    int fixedColorCount = m_curveColors.size();
    int curveCount = m_plotCurves.size();
    bool colorAssigned = false;
    for ( int i = 0; i < fixedColorCount; i++ ){
        bool colorAvailable = true;
        QString fixedColorName = m_curveColors[i].name();
        for ( int j = 0; j < curveCount; j++ ){
            if ( m_plotCurves[j]->getColor().name() == fixedColorName ){
                colorAvailable = false;
                break;
            }
        }
        if ( colorAvailable ){
            curveData->setColor( m_curveColors[i] );
            colorAssigned = true;
            break;
        }
    }

    //If there is no color in the fixed list, assign a random one.
    if ( !colorAssigned ){
        const int MAX_COLOR = 255;
        int redAmount = qrand() % MAX_COLOR;
        int greenAmount = qrand() % MAX_COLOR;
        int blueAmount = qrand() % MAX_COLOR;
        QColor randomColor( redAmount, greenAmount, blueAmount );
        curveData->setColor( randomColor.name());
    }
}


void Profiler::_clearData(){
    m_plotManager->clearData();
    int curveSize = m_plotCurves.size();
    for ( int i = curveSize - 1; i>= 0; i-- ){
        QString curveName = m_plotCurves[i]->getName();
        profileRemove( curveName );
    }
}

void Profiler::clearFits(){
    int curveSize = m_plotCurves.size();
    for ( int i = 0; i < curveSize; i++ ){
        m_plotCurves[i]->clearFit();
    }
    setGaussCount( 0 );
    setPolyCount( 0 );
    setShowFitResiduals( false );
    setShowFitStatistics( false );
    m_plotManager->clearDataFit();
}

std::vector<double> Profiler::_convertUnitsX( std::shared_ptr<CurveData> curveData,
        const QString& bottomUnit ) const {
    std::vector<double> converted = curveData->getValuesX();
    _convertDataX( converted, bottomUnit, curveData );
    return converted;
}

std::vector<double> Profiler::_convertUnitsXFit( std::shared_ptr<CurveData> curveData,
        const QString& bottomUnit ) const {
    std::vector<double> converted = curveData->getValuesXFit();
    _convertDataX( converted, bottomUnit, curveData );
    return converted;
}

std::vector<double>  Profiler::_convertUnitsXFitParams( std::shared_ptr<CurveData> curveData,
        const QString & bottomUnit ) const {
    std::vector<std::tuple<double,double,double> > fitParams = curveData->getFitParams();
    //Store center, center - fbhw, center, center-fbhw in a vector.
    int count = fitParams.size();
    std::vector<double> converted( count * 2 );
    for ( int i = 0; i < count; i++ ){
        converted[2*i] = std::get<0>( fitParams[i]);
        converted[2*i+1] = converted[2*i] - std::get<2>( fitParams[i]);
    }
    _convertDataX( converted, bottomUnit, curveData );
    return converted;
}

void Profiler::_convertDataX( std::vector<double>& converted, const QString& bottomUnit,
        std::shared_ptr<CurveData> curveData ) const {
    QString oldBottomUnit = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
    std::shared_ptr<Layer> dataSource = curveData->getLayer();
    if ( ! bottomUnit.isEmpty() ){
        QString newUnit = _getUnitUnits( bottomUnit );
        QString oldUnit = _getUnitUnits( oldBottomUnit );
        if ( newUnit != oldUnit ){
            _convertX ( converted, dataSource, oldUnit, newUnit );
        }
    }
}

void Profiler::_convertX( std::vector<double>& converted,
        std::shared_ptr<Layer> layer,
        const QString& oldUnit, const QString& newUnit ) const {
    if ( layer ){
        std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource = layer->_getImage();
        auto result = Globals::instance()-> pluginManager()
                             -> prepare <Carta::Lib::Hooks::ConversionSpectralHook>(dataSource,
                                     oldUnit, newUnit, converted );
        auto lam = [&converted] ( const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data ) {
            converted = data;
        };
        try {
            result.forEach( lam );
        }
        catch( char*& error ){
            QString errorStr( error );
            ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
            hr->registerError( errorStr );
        }
    }
}


std::vector<double> Profiler::_convertUnitsY( std::shared_ptr<CurveData> curveData, const QString& newUnits ) const {
    std::vector<double> converted = curveData->getValuesY();
    std::vector<double> convertedX = curveData->getValuesX();
    _convertDataY( converted, convertedX, curveData, newUnits );
    return converted;
}


void Profiler::_convertDataY( std::vector<double>& converted, const std::vector<double>& plotDataX, std::shared_ptr<CurveData> curveData,
        const QString& newUnits ) const {
    QString leftUnit = m_state.getValue<QString>( AXIS_UNITS_LEFT );
    Controller* controller = _getControllerSelected();
    if ( controller ){
        std::shared_ptr<Layer> layer = curveData->getLayer();
        if ( layer ){
            std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource = layer->_getImage();
            //First, we need to make sure the x-values are in Hertz.
            QString hertzKey = UnitsSpectral::NAME_FREQUENCY + "(" + UnitsFrequency::UNIT_HZ + ")";
            std::vector<double> hertzVals = plotDataX;
            _convertDataX( hertzVals, hertzKey, curveData );
            bool validBounds = false;
            std::pair<double,double> boundsY = m_plotManager->getPlotBoundsY( curveData->getName(), &validBounds );
            if ( validBounds ){
                QString maxUnit = m_plotManager->getAxisUnitsY();
                auto result = Globals::instance()-> pluginManager()
                                     -> prepare <Carta::Lib::Hooks::ConversionIntensityHook>(dataSource,
                                             leftUnit, newUnits, hertzVals, converted,
                                             boundsY.second, maxUnit );;

                auto lam = [&converted] ( const Carta::Lib::Hooks::ConversionIntensityHook::ResultType &data ) {
                    converted = data;
                };
                try {
                    result.forEach( lam );
                }
                catch( char*& error ){
                    QString errorStr( error );
                    ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
                    hr->registerError( errorStr );
                }
            }
        }
    }
}

void Profiler::_cursorUpdate( double x, double y ){
    QString cursorText;
    int curveCount = m_plotCurves.size();
    double approxError = 1.0;
    //Find the curve with a point closest to x,y.
    for ( int i = 0; i < curveCount; i++ ){
        double error = 0;
        QString curveText = m_plotCurves[i]->getCursorText( x, y, &error );
        if ( !curveText.isEmpty() ){
            if ( error < approxError ){
                cursorText = curveText;
                approxError = error;
            }
        }
    }
    m_plotManager->setCursorText( cursorText );
}


int Profiler::_findCurveIndex( const QString& name ) const {
    int curveCount = m_plotCurves.size();
    int index = -1;
    for ( int i = 0; i < curveCount; i++ ){
        if ( m_plotCurves[i]->getName() == name ){
            index = i;
            break;
        }
    }
    return index;
}


void Profiler::_fitFinished(const std::vector<Carta::Lib::Hooks::FitResult> & results){
    int resultCount = results.size();

    for ( int i = 0; i < resultCount; i++ ){
        Carta::Lib::Hooks::FitResult result = results[i];
        Carta::Lib::Fit1DInfo::StatusType statusType = result.getStatus();
        if ( statusType == Carta::Lib::Fit1DInfo::StatusType::COMPLETE ||
                statusType == Carta::Lib::Fit1DInfo::StatusType::PARTIAL ){
            std::vector<std::pair<double,double>> fitData = result.getData();

            int fitDataCount = fitData.size();
            std::vector<double> dataX( fitDataCount );
            std::vector<double> dataY( fitDataCount );
            double sum = 0;
            int count = 0;
            for ( int i = 0; i < fitDataCount; i++ ){
                dataX[i] = fitData[i].first;
                dataY[i] = fitData[i].second;
                if ( std::isfinite( dataY[i]) ){
                    sum = sum + dataY[i];
                    count++;
                }
            }
            //The x-data values come back in pixels.  We need to convert them to the x-units
            //we are currently using.
            int curveIndex =  _findCurveIndex( result.getName() );
            if ( curveIndex >= 0 ){
                QString axisUnits = getAxisUnitsX();
                QString unitStr = _getUnitUnits( axisUnits );
                if ( unitStr.length() > 0 ){
                    _convertX( dataX, m_plotCurves[curveIndex]->getLayer(), "", unitStr );
                }

                double mean = sum / count;
                double rms = result.getRMS();
                m_plotManager->setMarkedRange( mean - rms, mean + rms );
                m_plotManager->setHLinePosition( mean );

                m_plotCurves[ curveIndex ]->setFit( dataX, dataY );
                m_plotCurves[ curveIndex ]->setGaussParams( result.getGaussFits() );
                std::vector<double> polyCoeffs = result.getPolyCoefficients();
                m_plotCurves[ curveIndex ]->setFitPolyCoeffs( result.getPolyCoefficients() );
                QString statusMsg = _getFitStatusMessage(result.getStatus());
                m_plotCurves[ curveIndex ]->setFitStatus( statusMsg );
                QString lineStyle = getLineStyleFit();
                m_plotCurves[curveIndex] ->setLineStyleFit( lineStyle );
                _updatePlotData();
                _updateResidualData();
                _updateFitStatistics();
                //Restore any zoom imposed on the graph
                _updatePlotBounds();
            }

        }
        else if ( statusType == Carta::Lib::Fit1DInfo::StatusType::ERROR ){
            QString errorStr( "There was an error performing the fit." );
            ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
            hr->registerError( errorStr );
        }
        else {
            qDebug() << "Status of fit="<<(int)(statusType);
        }
    }
}

bool Profiler::_generateCurve( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region ){
    bool generates = false;
    if ( layer ){
		QString curveId = CurveData::_generateName( layer, region );
		int profileIndex = _findCurveIndex( curveId );
		if ( profileIndex < 0 ){
			_generateData( layer, region);
			generates = true;
		}
		else {
			//Set the curve active
			m_plotCurves[profileIndex]->setActive( true );
		}

    }
    return generates;
}




void Profiler::_generateData( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
        bool createNew ){
    QString id = CurveData::_generateName( layer, region );
    int curveIndex = _findCurveIndex( id );
    Carta::Lib::ProfileInfo profInfo;
    if ( curveIndex >= 0 ){
        profInfo = m_plotCurves[curveIndex]->getProfileInfo();
    }
    else {
    	//Provide profile info based on the current settings.
    	profInfo.setRestFrequency( getRestFrequency( ""));
    	QString stat = getStatistic( "" );
    	Carta::Lib::ProfileInfo::AggregateType aggType = this->m_stats->getTypeFor( stat );
    	profInfo.setAggregateType( aggType );
    	profInfo.setRestUnit( getRestUnits("") );
    }
    profInfo.setSpectralUnit( getSpectralUnits() );
    profInfo.setSpectralType( getSpectralType() );
    m_renderService->renderProfile(layer, region, profInfo, createNew );
}

QString Profiler::getSpectralUnits() const {
	QString xUnits = getAxisUnitsX();
	return _getUnitUnits( xUnits );
}

QString Profiler::getSpectralType() const {
	QString xUnits = getAxisUnitsX();
	QString typeStr = _getUnitType( xUnits );
	if ( typeStr == UnitsSpectral::NAME_FREQUENCY ){
		typeStr = "";
	}
	return typeStr;
}


void Profiler::_generateFit( ){
    //Get the curves we will fit.
    int curveCount = m_plotCurves.size();
    int selectCount = 0;
    for ( int i = 0; i < curveCount; i++ ){
        if ( m_plotCurves[i]->isSelectedFit() && !m_plotCurves[i]->_isPointSource()){
            selectCount++;
        }
    }
    if ( selectCount > 0 ) {
        std::vector<Carta::Lib::Fit1DInfo> fitInfos( selectCount );
        int polyCount = getPolyCount();
        int gaussCount = getGaussCount();
        if ( polyCount > 0 || gaussCount > 0 ){
            //If there are gaussians, make sure we have manual guesses in manual mode.
            //This may not be the case if there were previously no plot curves & hence,
            //no guesses.
            bool manualMode = isFitManualGuess();
            if ( gaussCount > 0 && manualMode ){
                int guessCount = getGuessCount();
                if ( guessCount != gaussCount ){
                    _makeInitialGuesses( gaussCount );
                    _resetFitGuessPixels();
                }
            }
            bool randomHeuristics = isRandomHeuristics();

            int j = 0;
            for ( int i = 0; i < curveCount; i++ ){
                if ( m_plotCurves[i]->isSelectedFit() && !m_plotCurves[i]->_isPointSource() ){
                    //Store the parameters used to fit the curve so that we can
                    //restore the state if we need to.
                    m_plotCurves[i]->setFitParams( m_stateFit.toString( CurveData::FIT) );
                    std::vector<double > curveData = m_plotCurves[i]->getValuesY();
                    QString name = m_plotCurves[i]->getName();
                    fitInfos[j].setId( name );
                    fitInfos[j].setData( curveData );
                    fitInfos[j].setPolyDegree( polyCount );
                    fitInfos[j].setGaussCount( gaussCount );
                    fitInfos[j].setRandomHeuristics( randomHeuristics );
                    if ( manualMode ){
                        fitInfos[j].setInitialGaussianGuesses( getFitGuesses());
                    }
                    j++;
                }
            }
            m_fitService->fitProfile(fitInfos );
        }
     }
}


std::vector<std::tuple<double,double,double> > Profiler::_generateFitGuesses( int count, bool random ){
    CARTA_ASSERT( count >= 0 );
    std::vector<std::tuple<double,double,double> > guesses(count);

    //Set up uniformly spaced guesses based on the first curve that has been
    //selected to fit.
    double xmin = 0;
    double xmax = 10;
    double ymin = 0;
    double ymax = 10;
    double xRange = xmax - xmin;
    double xStep = xRange / (count + 1);
    double yDecrease = .1;
    const double FBHW_MULT = 0.45;
    double fbhw = FBHW_MULT * xRange / (count + 1);
    int curveCount = m_plotCurves.size();
    for ( int i = 0; i < curveCount; i++ ){
        if ( m_plotCurves[i]->isSelectedFit() ){
            m_plotCurves[i]->getMinMax(&xmin, &xmax, &ymin, &ymax );
            xRange = xmax - xmin;
            xStep = xRange / (count + 1);
            fbhw = FBHW_MULT * xRange / (count + 1);
            break;
        }
    }
    //Store the guesses.
    double peak = ymax;
    for ( int i = 0; i < count; i++ ){
        double center = xmin + xStep * (i + 1);
        if ( random ){
            int randomIndex = qrand() % ((int)(xRange));
            center = randomIndex + xmin;
        }
        //Go down a fixed percentile each time.
        peak = peak * qPow(yDecrease,i);
        if ( random ){
            //Weighted average of the minimum and maximum.
            peak = 0.75 * ymax + 0.25 * ymin;
        }
        double endDist = qMin( center - xmin, xmax -center );
        double fbhwFit = qMin( fbhw, endDist);
        guesses[i] = std::tuple<double,double,double>( center, peak, fbhwFit );
    }
    return guesses;
}

QString Profiler::getAxisUnitsX() const {
    return m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
}

QString Profiler::getAxisUnitsY() const {
    return m_state.getValue<QString>( AXIS_UNITS_LEFT );
}

Controller* Profiler::_getControllerSelected() const {
    //We are only supporting one linked controller.
    Controller* controller = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink(i );
        Controller* control = dynamic_cast<Controller*>( obj);
        if ( control != nullptr){
            controller = control;
            break;
        }
    }
    return controller;
}

std::pair<double,double> Profiler::_getCurveRangeX() const {
    double maxValue = -1 * std::numeric_limits<double>::max();
    double minValue = std::numeric_limits<double>::max();
    int curveCount = m_plotCurves.size();
    for ( int i = 0; i < curveCount; i++ ){
        double curveMinValue = minValue;
        double curveMaxValue = maxValue;
        double yMin = minValue;
        double yMax = maxValue;
        m_plotCurves[i]->getMinMax( &curveMinValue,&curveMaxValue,&yMin,&yMax);
        if ( curveMinValue < minValue ){
            minValue = curveMinValue;
        }
        if ( curveMaxValue > maxValue ){
            maxValue = curveMaxValue;
        }
    }
    return std::pair<double,double>( minValue, maxValue );
}


std::vector<std::shared_ptr<Layer> > Profiler::_getDataForGenerateMode( Controller* controller) const {
    QString generateMode = m_state.getValue<QString>( GEN_MODE );
    std::vector<std::shared_ptr<Layer> > dataSources;
    if ( m_generateModes->isCurrent( generateMode ) ){
        std::shared_ptr<Layer> dataSource = controller->getLayer("");
        if ( dataSource ){
            dataSources.push_back( dataSource );
        }
    }
    else if ( m_generateModes->isAll( generateMode ) ){
        dataSources = controller->getLayers();
    }
    else if ( m_generateModes->isAllExcludeSingle( generateMode ) ){
        std::vector<std::shared_ptr<Layer> > dSources = controller->getLayers();
        int dCount = dSources.size();
        for ( int i = 0; i < dCount; i++ ){
            int specCount = dSources[i]->_getFrameCount( Carta::Lib::AxisInfo::KnownType::SPECTRAL );
            if ( specCount > 1 ){
                dataSources.push_back( dSources[i]);
            }
        }
    }
    return dataSources;
}


int Profiler::_getExtractionAxisIndex( std::shared_ptr<Carta::Lib::Image::ImageInterface> image ) const {
    int axis = Util::getAxisIndex( image, Carta::Lib::AxisInfo::KnownType::SPECTRAL );
    if ( axis < 0 ){
        //See if it has a tabular axis.
        axis = Util::getAxisIndex( image, Carta::Lib::AxisInfo::KnownType::TABULAR );
    }
    if ( axis < 0 ){
        //See if it has a linear axis.
        axis = Util::getAxisIndex( image, Carta::Lib::AxisInfo::KnownType::LINEAR );
    }
    return axis;
}

std::vector<std::tuple<double,double,double> > Profiler::getFitGuesses(){
    QString guessKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    int guessCount = m_stateFit.getArraySize( guessKey );
    std::vector<std::tuple<double,double,double> > guesses( guessCount );
    for ( int i = 0; i < guessCount; i++ ){
        QString indexKey = Carta::State::UtilState::getLookup( guessKey, i );
        QString centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER );
        double center = m_stateFit.getValue<double>( centerKey );
        QString peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK );
        double peak = m_stateFit.getValue<double>( peakKey );
        QString fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW );
        double fbhw = m_stateFit.getValue<double>( fbhwKey );
        std::tuple<double,double,double> guess( center, peak, fbhw );
        guesses[i] = guess;
    }
    return guesses;
}

QString Profiler::_getFitStatusMessage( Carta::Lib::Fit1DInfo::StatusType statType) const{
    QString statusStr( "");
    if ( statType == Carta::Lib::Fit1DInfo::StatusType::NOT_DONE ){
        statusStr = "Not performed.";
    }
    if ( statType == Carta::Lib::Fit1DInfo::StatusType::ERROR ){
        statusStr = "There was an error computing the fit.";
    }
    if ( statType == Carta::Lib::Fit1DInfo::StatusType::PARTIAL ){
        statusStr = "Partially computed.";
    }
    if ( statType == Carta::Lib::Fit1DInfo::StatusType::COMPLETE ){
        statusStr = "Completed.";
    }
    return statusStr;
}


int Profiler::getGaussCount() const {
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, GAUSS_COUNT );
    return m_stateFit.getValue<int>( key );
}

int Profiler::getGuessCount() const {
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    return m_stateFit.getArraySize( key );
}


QString Profiler::_getLegendLocationsId() const {
    return m_legendLocations->getPath();
}

QString Profiler::getLineStyleFit() const {
    return m_state.getValue<QString>( CurveData::STYLE_FIT );
}

QList<QString> Profiler::getLinks() const {
    return m_linkImpl->getLinkIds();
}


int Profiler::getPolyCount() const {
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, POLY_DEGREE );
    return m_stateFit.getValue<int>( key );
}


QString Profiler::_getPreferencesId() const {
    return m_preferences->getPath();
}

std::vector<std::shared_ptr<Region> > Profiler::_getRegionForGenerateMode() const {
    QString generateMode = m_state.getValue<QString>( GEN_MODE );
    std::vector<std::shared_ptr<Region> > regions;
    std::shared_ptr<RegionControls> regionControls( nullptr );
    Controller* controller = _getControllerSelected();
    if ( controller ){
    	regionControls = controller->getRegionControls();
    }
    if ( m_generateModes->isCurrent( generateMode ) ){
    	if ( regionControls ){
    		std::shared_ptr<Region> selectedRegion = regionControls->getRegion( "");
    		if ( selectedRegion ){
    			regions.push_back( selectedRegion );
    		}
    	}
    }
    else if ( m_generateModes->isAll( generateMode ) || m_generateModes->isAllExcludeSingle( generateMode ) ){
    	if ( regionControls ){
    		regions = regionControls->getRegions();
    	}
    }
    else {
    	qDebug() << "Unrecognized region generate mode: "<<generateMode;
    }
    return regions;
}

double Profiler::getRestFrequency( const QString& curveName ) const {
	double restFrequency = -1;
	int curveIndex = _findCurveIndex( curveName );
	if ( curveIndex >= 0 ){
		restFrequency = m_plotCurves[curveIndex]->getRestFrequency();
	}
	else {
		restFrequency = m_state.getValue<double>( CurveData::REST_FREQUENCY );
	}
	return restFrequency;
}

QString Profiler::getRestUnits( const QString& curveName ) const {
	QString restUnits;
	int curveIndex = _findCurveIndex( curveName );
	if ( curveIndex >= 0 ){
		restUnits = m_plotCurves[curveIndex]->getRestUnits();
	}
	else {
		restUnits = m_state.getValue<QString>( REST_UNITS );
	}
	return restUnits;
}


QString Profiler::getSelectedLayer() const {
	return m_stateData.getValue<QString>( IMAGE_SELECT );
}

QString Profiler::getSelectedRegion() const {
	return m_stateData.getValue<QString>( REGION_SELECT );
}

QString Profiler::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_preferences->getStateString(sessionId, type) );
        prefState.insertValue<QString>( CurveData::FIT, m_stateFit.toString());
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
    }
    return result;
}

QString Profiler::getStatistic( const QString& curveName ) const {
	QString stat;
		int curveIndex = _findCurveIndex( curveName );
		if ( curveIndex >= 0 ){
			stat = m_plotCurves[curveIndex]->getStatistic();
		}
		else {
			stat = m_state.getValue<QString>( CurveData::STATISTIC );
		}
		return stat;
}

QString Profiler::_getUnitType( const QString& unitStr ){
    QString unitType = unitStr;
    int unitStart = unitStr.indexOf( "(");
    if ( unitStart >= 0 ){
        unitType = unitStr.mid( 0, unitStart );
    }
    return unitType;
}


QString Profiler::_getUnitUnits( const QString& unitStr ){
    QString strippedUnit = "";
    int unitStart = unitStr.indexOf( "(");
    if ( unitStart >= 0 ){
        int substrLength = unitStr.length() - unitStart - 2;
        if ( substrLength > 0){
            strippedUnit = unitStr.mid( unitStart + 1, substrLength );
        }
    }
    return strippedUnit;
}

double Profiler::getZoomMax() const {
    return m_stateData.getValue<double>( ZOOM_MAX );
}

double Profiler::getZoomMin() const {
    return m_stateData.getValue<double>( ZOOM_MIN );
}

double Profiler::getZoomMinPercent() const {
    return m_stateData.getValue<double>( ZOOM_MIN_PERCENT );
}

double Profiler::getZoomMaxPercent() const {
    return m_stateData.getValue<double>( ZOOM_MAX_PERCENT );
}


void Profiler::_initializeDefaultState(){
    //Data state is the curves
    m_stateData.insertArray( CURVES, 0 );
    m_stateData.insertValue<QString>(CURVE_SELECT, "" );
    m_stateData.insertValue<QString>( IMAGE_SELECT, "" );
    m_stateData.insertValue<QString>( REGION_SELECT, NO_REGION );
    m_stateData.insertValue<double>( ZOOM_MIN, 0 );
    m_stateData.insertValue<double>(ZOOM_MAX, 1);
    m_stateData.insertValue<double>(ZOOM_MIN_PERCENT, 0);
    m_stateData.insertValue<double>(ZOOM_MAX_PERCENT, 100 );
    m_stateData.insertValue<bool>(ZOOM_BUFFER, false );
    m_stateData.insertValue<double>( ZOOM_BUFFER_SIZE, 10 );
    m_stateData.flushState();

    //Default units
    QString bottomUnit = m_spectralUnits->getDefault();
    QString unitType = _getUnitType( bottomUnit );
    m_plotManager->setTitleAxisX( unitType );
    m_state.insertValue<bool>(AUTO_GENERATE, true );
    m_state.insertValue<QString>( AXIS_UNITS_BOTTOM, bottomUnit );
    m_state.insertValue<QString>( AXIS_UNITS_LEFT, m_intensityUnits->getDefault());
    m_state.insertValue<QString>(GEN_MODE, m_generateModes->getDefault());
    m_state.insertValue<QString>( CurveData::STYLE_FIT, m_lineStyles->getDefaultSecondary());

    //For creating new profiles
    m_state.insertValue<QString>(CurveData::STATISTIC, m_stats->getDefault());
    m_state.insertValue<double>(CurveData::REST_FREQUENCY, 0 );
    m_state.insertValue<QString>( REST_UNITS, m_frequencyUnits->getDefault());
    m_state.insertValue<bool>(CurveData::REST_FREQUENCY_UNITS, true );
    m_state.insertValue<QString>( Util::NAME, "");

    //Legend
    bool external = false;
    QString legendLoc = m_legendLocations->getDefaultLocation( external );
    m_state.insertValue<QString>( LEGEND_LOCATION, legendLoc );
    m_state.insertValue<bool>( LEGEND_EXTERNAL, external );
    m_state.insertValue<bool>( LEGEND_SHOW, true );
    m_state.insertValue<bool>( LEGEND_LINE, true );

    //Plot
    m_state.insertValue<bool>(GRID_LINES, false );

    //Default Tab
    m_state.insertValue<int>( Util::TAB_INDEX, 2 );
    m_state.insertValue<bool>( SHOW_CURSOR, false );

    //Significant digits.
    m_state.insertValue<int>(Util::SIGNIFICANT_DIGITS, 6 );

    //Fit show/hide on display
    m_state.insertValue<bool>( SHOW_RESIDUALS, false );
    m_state.insertValue<bool>( SHOW_GUESSES, false );
    m_state.insertValue<bool>( SHOW_STATISTICS, true );
    m_state.insertValue<bool>( SHOW_MEAN_RMS, false );
    m_state.insertValue<bool>( SHOW_FRAME, true );
    m_state.insertValue<bool>( SHOW_PEAK_LABELS, false );
    m_state.flushState();

    //Fit Parameters
    m_stateFit.insertValue<int>( PLOT_WIDTH, 0 );
    m_stateFit.insertValue<int>( PLOT_HEIGHT, 0 );
    m_stateFit.insertValue<int>( PLOT_LEFT, 0 );
    m_stateFit.insertValue<int>( PLOT_TOP, 0 );
    m_stateFit.insertObject( CurveData::FIT );
    QString gaussKey = Carta::State::UtilState::getLookup( CurveData::FIT, GAUSS_COUNT );
    m_stateFit.insertValue<int>( gaussKey, 0 );
    QString polyKey = Carta::State::UtilState::getLookup( CurveData::FIT, POLY_DEGREE );
    m_stateFit.insertValue<int>( polyKey, 0 );
    QString heurKey = Carta::State::UtilState::getLookup( CurveData::FIT, HEURISTICS );
    m_stateFit.insertValue<bool>( heurKey, true );
    QString manKey = Carta::State::UtilState::getLookup( CurveData::FIT, MANUAL_GUESS );
    m_stateFit.insertValue<bool>( manKey, false );
    QString guessesKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    m_stateFit.insertArray( guessesKey, 0 );
    m_stateFit.flushState();

    m_stateFitStatistics.insertValue<QString>( FIT_STATISTICS, "" );
    m_stateFitStatistics.flushState();
    _updatePlotDisplay();
}


void Profiler::_initializeCallbacks(){

    addCommandCallback( "clearFits", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        clearFits();
        return "";
    });

    addCommandCallback( "registerLegendLocations", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = _getLegendLocationsId();
        return result;
    });


    addCommandCallback( "registerController", [=] (const QString & /*cmd*/,
                           const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                   QString stackId;
                   Controller* controller = _getControllerSelected();
                   if ( controller != nullptr ){
                       stackId = controller->_getStackId();
                   }
                   return stackId;
               });

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = _getPreferencesId();
        return result;
    });

    addCommandCallback( "registerRegionControls", [=] (const QString & /*cmd*/,
    		const QString & /*params*/, const QString & /*sessionId*/) -> QString {
    	QString controlId;
    	Controller* controller = _getControllerSelected();
    	if ( controller != nullptr ){
    		std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
    		if ( regionControls ){
    			controlId = regionControls->getPath();
    		}
    	}
    	return controlId;
    });

    addCommandCallback( "resetInitialFitGuesses", [=] (const QString & /*cmd*/,
    		const QString & /*params*/, const QString & /*sessionId*/) -> QString {
    	resetInitialFitGuesses();
            return "";
        });

    	addCommandCallback( "setAutoGenerate", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
               QString result;
               std::set<QString> keys = {AUTO_GENERATE};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString generateStr = dataValues[*keys.begin()];
               bool validBool = false;
               bool autoGen = Util::toBool( generateStr, &validBool );
               if ( validBool ){
                   setAutoGenerate( autoGen );
               }
               else {
                   result = "Whether or not to automatically generate profiles must be true/false: " + params;
               }
               Util::commandPostProcess( result );
               return result;
           });

    addCommandCallback( "setAxisUnitsBottom", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::UNITS};

        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString unitStr = dataValues[*keys.begin()];
        QString result = setAxisUnitsX( unitStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAxisUnitsLeft", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::UNITS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString unitStr = dataValues[*keys.begin()];
        QString result = setAxisUnitsY( unitStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setFitManualGuesses", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {CurveData::INITIAL_GUESSES};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString guessStr = dataValues[*keys.begin()];
            QStringList guessList = guessStr.split( " ");
            const int GUESS_SIZE = 3;
            int guessCount = guessList.size() / GUESS_SIZE;
            std::vector<std::tuple<int,int,int> > guesses( guessCount);
            QString errorMsg = "Initial fit manual guesses must be valid integers: "+params;
            for ( int i = 0; i < guessCount; i++ ){
                bool validCenter = false;
                bool validPeak = false;
                bool validFBHW = false;
                //Screen coordinates of a guess.
                int centerPixel = guessList[i*GUESS_SIZE].toInt(&validCenter );
                int peakPixel = guessList[i*GUESS_SIZE+1].toInt(&validPeak );
                int fbhwPixel = guessList[i*GUESS_SIZE+2].toInt(&validFBHW );
                if ( !validCenter || !validPeak || !validFBHW ){
                    result = errorMsg;
                    break;
                }
                guesses[i] = std::tuple<int,int,int>(centerPixel, peakPixel,fbhwPixel);
            }
            if ( result.isEmpty() ){
                setFitInitialGuessesPixels( guesses );
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setShowCursor", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {SHOW_CURSOR};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString cursorStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool cursor = Util::toBool( cursorStr, &validBool );
            if ( validBool ){
                setShowCursor( cursor );
            }
            else {
                result = "Whether or not to show the cursor position must be true/false: " + params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setShowFrame", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
               QString result;
               std::set<QString> keys = {SHOW_FRAME};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString frameStr = dataValues[*keys.begin()];
               bool validBool = false;
               bool frame = Util::toBool( frameStr, &validBool );
               if ( validBool ){
                   setShowFrame( frame );
               }
               else {
                   result = "Whether or not to show the current frame must be true/false: " + params;
               }
               Util::commandPostProcess( result );
               return result;
           });

    addCommandCallback( "setManualGuess", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {MANUAL_GUESS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString guessStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool manualGuess = Util::toBool( guessStr, &validBool );
        if ( validBool ){
            setFitManualGuess( manualGuess );
        }
        else {
            result = "Whether or not to manually specify fit initial conditions must be true/false: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setShowMeanRMS", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {SHOW_MEAN_RMS};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString showStr = dataValues[*keys.begin()];
           bool validBool = false;
           bool show = Util::toBool( showStr, &validBool );
           if ( validBool ){
               setShowMeanRMS( show );
           }
           else {
               result = "Whether or not to show fit mean & RMS must be true/false: " + params;
           }
           Util::commandPostProcess( result );
           return result;
       });

    addCommandCallback( "setShowPeakLabels", [=] (const QString & /*cmd*/,
                  const QString & params, const QString & /*sessionId*/) -> QString {
              QString result;
              std::set<QString> keys = {SHOW_PEAK_LABELS};
              std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
              QString showStr = dataValues[*keys.begin()];
              bool validBool = false;
              bool show = Util::toBool( showStr, &validBool );
              if ( validBool ){
                  setShowPeakLabels( show );
              }
              else {
                  result = "Whether or not to show peak labels must be true/false: " + params;
              }
              Util::commandPostProcess( result );
              return result;
          });

    addCommandCallback( "setShowResiduals", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {SHOW_RESIDUALS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString residualStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool residuals = Util::toBool( residualStr, &validBool );
        if ( validBool ){
            setShowFitResiduals( residuals );
        }
        else {
            result = "To show fit residuals must be true/false: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setShowGuesses", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {SHOW_GUESSES};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString guessStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool guesses = Util::toBool( guessStr, &validBool );
            if ( validBool ){
                result = setShowFitGuesses( guesses );
            }
            else {
                result = "Whether or not to show fit manual guesses must be true/false: " + params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setShowStatistics", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {SHOW_STATISTICS};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString statStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool stats = Util::toBool( statStr, &validBool );
            if ( validBool ){
                setShowFitStatistics( stats );
            }
            else {
                result = "To show fit residuals must be true/false: " + params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setZoomBufferSize", [=] (const QString & /*cmd*/,
                      const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {ZOOM_BUFFER_SIZE};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString zoomBufferStr = dataValues[*keys.begin()];
           bool validInt = false;
           double zoomBuffer = zoomBufferStr.toInt( &validInt );
           if ( validInt ){
               result = setZoomBufferSize( zoomBuffer );
           }
           else {
               result = "Invalid zoom buffer size: " + params+" must be a valid integer.";
           }
           Util::commandPostProcess( result );
           return result;
       });

    addCommandCallback( "setRandomHeuristics", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {HEURISTICS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString heuristicsStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool heuristics = Util::toBool(heuristicsStr, &validBool );
        if ( validBool ){
            setRandomHeuristics( heuristics );
        }
        else {
            result = "Whether or not to use random heuristics must be true/false: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setZoomBuffer", [=] (const QString & /*cmd*/,
                  const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
          std::set<QString> keys = {ZOOM_BUFFER};
          std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
          QString zoomBufferStr = dataValues[*keys.begin()];
          bool validBool = false;
          bool zoomBuffer = Util::toBool(zoomBufferStr, &validBool );
          if ( validBool ){
              setZoomBuffer( zoomBuffer );
          }
          else {
              result = "Use zoom buffer must be true/false: " + params;
          }
          Util::commandPostProcess( result );
          return result;
      });

    addCommandCallback( "setCurveName", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::NAME, "oldName"};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString nameStr = dataValues[Util::NAME];
            QString idStr = dataValues["oldName"];
            QString result = setCurveName( idStr, nameStr );
            Util::commandPostProcess( result );
            return result;
        });
    addCommandCallback( "setSelectedCurve", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::NAME};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString name = dataValues[Util::NAME];
            QString result = setSelectedCurve( name );
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setSelectedLayer", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString name = dataValues[Util::NAME];
        QString result = setSelectedLayer( name );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setSelectedRegion", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::NAME};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString name = dataValues[Util::NAME];
            QString result = setSelectedRegion( name );
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setFitCurves", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {"fitCurves"};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString names = dataValues[*keys.begin()];
                QStringList nameList = names.split( ";");
                QString result = setFitCurves( nameList );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setGenerationMode", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {"mode"};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString modeStr = dataValues[*keys.begin()];
                QString result = setGenerateMode( modeStr );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setRestFrequency", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME, CurveData::REST_FREQUENCY};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString nameStr = dataValues[Util::NAME];
        QString restFreqStr = dataValues[CurveData::REST_FREQUENCY];
        bool validDouble = false;
        double restFreq = restFreqStr.toDouble( &validDouble );
        QString result;
        if ( validDouble ){
            result = setRestFrequency( restFreq, nameStr );
        }
        else {
            result = "Rest frequency must be a valid number: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setRestUnit", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME, CurveData::REST_UNIT_FREQ};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString nameStr = dataValues[Util::NAME];
        QString restUnits = dataValues[CurveData::REST_UNIT_FREQ];
        QString result = setRestUnits( restUnits, nameStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setRestUnitType", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME, CurveData::REST_FREQUENCY_UNITS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString nameStr = dataValues[Util::NAME];
        QString restUnitsStr = dataValues[CurveData::REST_FREQUENCY_UNITS];
        bool validBool = false;
        bool restUnitsFreq = Util::toBool( restUnitsStr, &validBool );
        QString result;
        if ( validBool ){
            result = setRestUnitType( restUnitsFreq, nameStr );
        }
        else {
            result = "Rest unit type frequency must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "newProfile", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                QString result = profileNew();
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "removeProfile", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString nameStr = dataValues[Util::NAME];
        QString result = profileRemove( nameStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "resetRestFrequency", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString nameStr = dataValues[Util::NAME];
        QString result = resetRestFrequency( nameStr );
        Util::commandPostProcess( result );
        return result;
    });


    addCommandCallback( "setCurveColor", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE, Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString redStr = dataValues[Util::RED];
        QString greenStr = dataValues[Util::GREEN];
        QString blueStr = dataValues[Util::BLUE];
        QString curveName = dataValues[Util::NAME];
        bool validRed = false;
        int redAmount = redStr.toInt( &validRed );
        bool validGreen = false;
        int greenAmount = greenStr.toInt( &validGreen );
        bool validBlue = false;
        int blueAmount = blueStr.toInt( &validBlue );
        if ( validRed && validGreen && validBlue ){
            QStringList resultList = setCurveColor( curveName, redAmount, greenAmount, blueAmount );
            result = resultList.join( ";");
        }
        else {
            result = "Please check that curve colors are integers: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setGridLines", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {GRID_LINES};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString gridStr = dataValues[GRID_LINES];
            bool validBool = false;
            bool gridLines = Util::toBool( gridStr, &validBool );
            QString result;
            if ( validBool ){
                setGridLines( gridLines );
            }
            else {
                result = "Set toggling plot grid lines must be true/false: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setGaussCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {GAUSS_COUNT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString countStr = dataValues[GAUSS_COUNT];
        bool validCount = false;
        int count = countStr.toInt( &validCount );
        if ( validCount ){
            result = setGaussCount( count );
        }
        else {
            result = "Profile fit gaussian count must be an integer: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLegendLocation", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {LEGEND_LOCATION};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString locationStr = dataValues[LEGEND_LOCATION];
        QString result = setLegendLocation( locationStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLegendExternal", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {LEGEND_EXTERNAL};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString externalStr = dataValues[LEGEND_EXTERNAL];
        bool validBool = false;
        bool externalLegend = Util::toBool( externalStr, &validBool );
        QString result;
        if ( validBool ){
            setLegendExternal( externalLegend );
        }
        else {
            result = "Setting the legend external to the plot must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLegendShow", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {LEGEND_SHOW};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString showStr = dataValues[LEGEND_SHOW];
        bool validBool = false;
        bool show = Util::toBool( showStr, &validBool );
        QString result;
        if ( validBool ){
            setLegendShow( show );
        }
        else {
            result = "Set show legend must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLegendLine", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {LEGEND_LINE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString showStr = dataValues[LEGEND_LINE];
        bool validBool = false;
        bool show = Util::toBool( showStr, &validBool );
        QString result;
        if ( validBool ){
            setLegendLine( show );
        }
        else {
            result = "Set show legend line must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLineStyle", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::STYLE, Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString lineStyle = dataValues[Util::STYLE];
        QString curveName = dataValues[Util::NAME];
        QString result = setLineStyle( curveName, lineStyle );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLineStyleFit", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {CurveData::STYLE_FIT};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString lineStyle = dataValues[CurveData::STYLE_FIT];
            QString result = setLineStyleFit( lineStyle );
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setPolyDegree", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {POLY_DEGREE};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString countStr = dataValues[POLY_DEGREE];
            bool validCount = false;
            int count = countStr.toInt( &validCount );
            if ( validCount ){
                result = setPolyCount( count );
            }
            else {
                result = "Profile fit polynomial count must be an integer: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setPlotStyle", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
           std::set<QString> keys = {Util::STYLE, Util::NAME};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString plotStyle = dataValues[Util::STYLE];
           QString curveName = dataValues[Util::NAME];
           QString result = setPlotStyle( curveName, plotStyle );
           Util::commandPostProcess( result );
           return result;
       });

    addCommandCallback( "setSignificantDigits", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
               QString result;
               std::set<QString> keys = {Util::SIGNIFICANT_DIGITS};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString digitsStr = dataValues[Util::SIGNIFICANT_DIGITS];
               bool validDigits = false;
               int digits = digitsStr.toInt( &validDigits );
               if ( validDigits ){
                   result = setSignificantDigits( digits );
               }
               else {
                   result = "Profile significant digits must be an integer: "+params;
               }
               Util::commandPostProcess( result );
               return result;
           });

    addCommandCallback( "setStatistic", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
               std::set<QString> keys = { CurveData::STATISTIC, Util::NAME};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString statStr = dataValues[CurveData::STATISTIC];
               QString curveName = dataValues[Util::NAME];
               QString result = setStatistic( statStr, curveName );
               Util::commandPostProcess( result );
               return result;
           });

    addCommandCallback( "setTabIndex", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::TAB_INDEX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString tabIndexStr = dataValues[Util::TAB_INDEX];
        bool validIndex = false;
        int tabIndex = tabIndexStr.toInt( &validIndex );
        if ( validIndex ){
            result = setTabIndex( tabIndex );
        }
        else {
            result = "Please check that the tab index is a number: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setZoomRange", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {ZOOM_MIN, ZOOM_MAX};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString zoomMaxStr = dataValues[ZOOM_MAX];
           bool validRangeMax = false;
           double zoomMax = zoomMaxStr.toDouble( &validRangeMax );
           QString zoomMinStr = dataValues[ZOOM_MIN];
           bool validRangeMin = false;
           double zoomMin = zoomMinStr.toDouble( &validRangeMin );
           if ( !validRangeMax || !validRangeMin ){
               result = "Invalid profile range: " + params+"; bounds must be valid number(s).";
           }
           else {
               result = setZoomRange( zoomMin, zoomMax );
           }
           Util::commandPostProcess( result );
           return result;
       });

       addCommandCallback( "setZoomRangePercent", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
          QString result;

          std::set<QString> keys = {ZOOM_MIN_PERCENT, ZOOM_MAX_PERCENT};
          std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
          QString zoomMaxPercentStr = dataValues[ZOOM_MAX_PERCENT];
          bool validRangeMax = false;
          double zoomMaxPercent = zoomMaxPercentStr.toDouble( &validRangeMax );
          QString zoomMinPercentStr = dataValues[ZOOM_MIN_PERCENT];
          bool validRangeMin = false;
          double zoomMinPercent = zoomMinPercentStr.toDouble( &validRangeMin );

          if ( !validRangeMin || !validRangeMax ){
              result = "Invalid profile zoom percent range: " + params+"; must be valid number(s).";
          }
          else {
              result = setZoomRangePercent( zoomMinPercent, zoomMaxPercent );
          }
          Util::commandPostProcess( result );
          return result;
       });

       addCommandCallback( "zoomFull", [=] (const QString & /*cmd*/,
               const QString & /*params*/, const QString & /*sessionId*/) -> QString {
           QString result = setZoomRangePercent( 0, 100);
           Util::commandPostProcess( result );
           return result;
       });
}


void Profiler::_initializeStatics(){
    if ( m_spectralUnits == nullptr ){
        m_spectralUnits = Util::findSingletonObject<UnitsSpectral>();
    }
    if ( m_intensityUnits == nullptr ){
        m_intensityUnits = Util::findSingletonObject<UnitsIntensity>();
    }
    if ( m_generateModes == nullptr ){
        m_generateModes = Util::findSingletonObject<GenerateModes>();
    }
    if ( m_stats == nullptr ){
        m_stats = Util::findSingletonObject<ProfileStatistics>();
    }
    if ( m_lineStyles == nullptr ){
        m_lineStyles = Util::findSingletonObject<LineStyles>();
    }
    if ( m_frequencyUnits == nullptr ){
    	m_frequencyUnits = Util::findSingletonObject<UnitsFrequency>();
    }
    if ( m_wavelengthUnits == nullptr ){
    	m_wavelengthUnits = Util::findSingletonObject<UnitsWavelength>();
    }
}

bool Profiler::isAutoGenerate() const {
	return m_state.getValue<bool>( AUTO_GENERATE );
}

bool Profiler::isFitManualGuess( ) const {
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, MANUAL_GUESS );
    return m_stateFit.getValue<bool>( key );
}


bool Profiler::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


bool Profiler::isRandomHeuristics() const {
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, HEURISTICS );
    return m_stateFit.getValue<bool>( key );
}

bool Profiler::isShowPeakLabels() const {
    return m_state.getValue<bool>( SHOW_PEAK_LABELS );
}

bool Profiler::isShowResiduals() const {
    return m_state.getValue<bool>( SHOW_RESIDUALS );
}


void Profiler::_loadProfile( Controller* controller ){
    if( ! controller) {
        return;
    }
    m_plotManager->clearData();
    bool generatingNew = _updateProfiles( controller );
    //If we removed some curves but did not generate any new ones, the plot
    //needs to get updated (it will be updated automatically if a new curve is generated.
    if ( !generatingNew ){
        _updatePlotData();
    }
}

void Profiler::_makeInitialGuesses( int count ){
    QString guessKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    int oldCount = m_stateFit.getArraySize( guessKey );
    int diffCount = count - oldCount;
    if ( diffCount != 0 ){
        //Update the initial guess count in the curves.
        m_stateFit.resizeArray( guessKey, count, Carta::State::StateInterface::PreserveAll );
    }
    if ( diffCount > 0 ){
        //Set up uniformly spaced guesses based on the first curve that has been
        //selected to fit.
        bool random = true;
        if ( oldCount == 0 ){
            random = false;
        }

        std::vector<std::tuple<double,double,double> > guesses = _generateFitGuesses( diffCount, random );
        //Store the guesses.
        for ( int i = oldCount; i < count; i++ ){
            QString indexKey = Carta::State::UtilState::getLookup( guessKey, i );
            m_stateFit.setObject( indexKey );
            QString centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER );
            m_stateFit.insertValue<double>( centerKey, std::get<0>(guesses[i-oldCount]) );
            QString peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK );
            m_stateFit.insertValue<double>( peakKey, std::get<1>(guesses[i-oldCount]) );
            QString fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW );
            m_stateFit.insertValue<double>( fbhwKey, std::get<2>(guesses[i-oldCount]) );

            //Pixels
            centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER_PIXEL );
            m_stateFit.insertValue<int>( centerKey, 0 );
            peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK_PIXEL );
            m_stateFit.insertValue<int>( peakKey, 0 );
            fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW_PIXEL );
            m_stateFit.insertValue<int>( fbhwKey, 1 );
        }
    }
}

void Profiler::_movieFrame(){
    //Get the new frame from the plot
    bool valid = false;
    double xLocation = qRound( m_plotManager -> getVLinePosition(&valid));
    if ( valid ){
        //Need to convert the xLocation to a frame number.
        if ( m_plotCurves.size() > 0 ){
            std::vector<double> val(1);
            val[0] = xLocation;
            QString oldUnits = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
            QString basicUnit = _getUnitUnits( oldUnits );
            if ( !basicUnit.isEmpty() ){
                _convertX( val, m_plotCurves[0]->getLayer(), basicUnit, "");
            }
            Controller* controller = _getControllerSelected();
            if ( controller && m_timerId == 0 ){
                int oldFrame = controller->getFrame( Carta::Lib::AxisInfo::KnownType::SPECTRAL );
                if ( oldFrame != val[0] ){
                    m_oldFrame = oldFrame;
                    m_currentFrame = val[0];
                    m_timerId = startTimer( 1000 );
                }
            }
        }
    }
}

void Profiler::_plotSizeChanged(){
    QSize plotSize = m_plotManager->getPlotSize();
    m_stateFit.setValue<int>(PLOT_WIDTH, plotSize.width());
    m_stateFit.setValue<int>(PLOT_HEIGHT, plotSize.height());
    QPointF upperLeft = m_plotManager->getPlotUpperLeft();
    m_stateFit.setValue<int>(PLOT_LEFT, upperLeft.x() );
    m_stateFit.setValue<int>(PLOT_TOP, upperLeft.y() );
    _resetFitGuessPixels();
}

QString Profiler::profileNew(){
    QString result;
    Controller* controller = _getControllerSelected();
    if ( controller){
    	QString layerId = getSelectedLayer();
        std::shared_ptr<Layer> layer = controller->getLayer( layerId );
        std::shared_ptr<Region> region( nullptr );
        std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
        if ( regionControls ){
        	QString regionId = getSelectedRegion();
        	region = regionControls->getRegion( regionId );
        }
        if ( layer ){
			//Get the name that is stored.  If it matches an existing profile, just set
        	//the existing profile active.  Otherwise, generate a new one.
			Carta::Lib::ProfileInfo profInfo = CurveData::_generateProfileInfo( getRestFrequency(""), getRestUnits(""),
				getStatistic( ""), getSpectralType(), getSpectralUnits() );
			int curveCount = m_plotCurves.size();
			int curveIndex = -1;
			for ( int i = 0; i < curveCount; i++ ){
				bool match = m_plotCurves[i]->isMatch( layer, region, profInfo );
				if ( match ){
					curveIndex = i;
					break;
				}
			}
			if ( curveIndex >= 0 ){
				m_plotCurves[curveIndex]->setActive( true );
			}
			else {
				_generateData( layer, region, true );
			}
        }
        else {
        	result = "Cannot render without a layer";
        }
    }
    else {
        result = "Could not generate a profile - no linked images.";
    }
    return result;
}


QString Profiler::profileRemove( const QString& name ){
    QString result;
    int curveIndex = _findCurveIndex( name );
    if ( curveIndex >= 0 ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        objMan->removeObject( m_plotCurves[curveIndex]->getId());
        m_plotCurves.removeAt( curveIndex );
        m_plotManager->removeData( name );
        _saveCurveState();
        _updateZoomRangeBasedOnPercent();
        _updatePlotData();
    }
    else {
        result = "Could not find profile curve "+name+" to remove.";
    }
    return result;
}


void Profiler::_profileRendered(const Carta::Lib::Hooks::ProfileResult& result,
        std::shared_ptr<Layer> layer, std::shared_ptr<Region> region, bool createNew ){
    QString errorMessage = result.getError();
    if ( !errorMessage.isEmpty() ){
        ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
        hr->registerError( errorMessage );
    }
    else {
        std::vector< std::pair<double,double> > data = result.getData();
        int dataCount = data.size();
        if ( dataCount > 0 ){
            std::vector<double> plotDataX( dataCount );
            std::vector<double> plotDataY( dataCount );
            for( int i = 0 ; i < dataCount; i ++ ){
                plotDataX[i] = data[i].first;
                plotDataY[i] = data[i].second;
            }

            std::shared_ptr<CurveData> profileCurve( nullptr );
            QString id = CurveData::_generateName( layer, region );
            int curveIndex = _findCurveIndex( id );
            if ( curveIndex < 0 || createNew ){
                Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
                profileCurve.reset( objMan->createObject<CurveData>() );
                double restFrequency = result.getRestFrequency();
                int significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
                double restRounded = Util::roundToDigits( restFrequency, significantDigits );
                QString restUnit = result.getRestUnits();
                profileCurve->setRestQuantity( restRounded, restUnit );
                profileCurve->setSpectralInfo( getSpectralType(), getSpectralUnits() );
                _assignColor( profileCurve );
                if ( curveIndex < 0 ){
                    m_plotCurves.append( profileCurve );
                }
                else if ( createNew ){
                	//It will have the same name as an existing curve.  So we set a custom name
                	//be appending a number after the default name.
                	QString curveName = CurveData::_generateName( layer, region );
                	int existIndex = _findCurveIndex( curveName );
                	int i = 1;
                	while ( existIndex >= 0 ){
                		curveName = curveName + QString::number(i);
                		existIndex = _findCurveIndex( curveName );
                		i++;
                	}
                	profileCurve->setName( curveName );
                	m_plotCurves.append( profileCurve );
                }
                else {
                    m_plotCurves.replace( curveIndex, profileCurve );
                }
                profileCurve->setLayer( layer );
                profileCurve->setRegion( region );
            }
            else {
                profileCurve = m_plotCurves[curveIndex];
            }

            profileCurve->setData( plotDataX, plotDataY );
            _updateSelectedCurve();
            _saveCurveState();
            _generateFit();
            _updateZoomRangeBasedOnPercent();
            _updatePlotBounds();
            _updatePlotData();
        }
    }
}

void Profiler::refreshState(){
	CartaObject::refreshState();
	m_stateData.refreshState();
	m_stateFit.refreshState();
	m_stateFitStatistics.refreshState();
}

void Profiler::_removeUnsupportedCurves(){
	//If the image or the region used by the profile is not longer present, remove
	//the profile.
	Controller* controller = _getControllerSelected();
	if ( controller ){
		std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
		int curveCount = m_plotCurves.size();
		std::vector<int> removeIndices;
		for ( int i = 0; i < curveCount; i++ ){
			std::shared_ptr<Layer> layer = m_plotCurves[i]->getLayer();
			QString layerName = layer->_getLayerName();
			std::shared_ptr<Layer> existingLayer = controller->getLayer(layerName );
			if ( existingLayer ){
				//Check if region is still supported.
				if ( regionControls ){
					std::shared_ptr<Region> region = m_plotCurves[i]->getRegion();
					if ( region ){
						QString regionName = region->getRegionName();
						std::shared_ptr<Region> existingRegion = regionControls->getRegion( regionName );
						if (!existingRegion ){
							removeIndices.push_back( i );
						}
					}
				}
			}
			else {
				removeIndices.push_back( i );
			}
		}
		//Go through the plot curves and remove the ones marked for removal
		int removeCount = removeIndices.size();
		for ( int i = removeCount - 1; i >= 0; i-- ){
			m_plotCurves.removeAt( removeIndices[i] );
		}
	}
	else {
		//No controller, we should not have any curves.
		m_plotCurves.clear();
	}
}


QString Profiler::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
            _clearData();
        }
    }
    else {
       result = "Profiler was unable to remove link only image links are supported";
    }
    return result;
}

void Profiler::_resetFitGuessPixels(){
    //Update the corresponding pixels.
    QString guessKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    int guessCount = m_stateFit.getArraySize( guessKey );
    for ( int i = 0; i < guessCount; i++ ){
        QString indexKey = Carta::State::UtilState::getLookup( guessKey, i );
        QString centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER );
        double center = m_stateFit.getValue<double>( centerKey );
        QString peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK );
        double peak = m_stateFit.getValue<double>( peakKey );
        QString fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW );
        double fbhw = m_stateFit.getValue<double>( fbhwKey );
        bool validCenter = false;
        QPointF centerPt = m_plotManager->getScreenPoint( QPointF(center,peak),&validCenter);
        bool validOffset = false;
        QPointF offsetPt = m_plotManager->getScreenPoint( QPointF(center - fbhw,peak), &validOffset );
        if ( validCenter && validOffset ){
            centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER_PIXEL );
            m_stateFit.setValue<int>( centerKey, (int)(centerPt.x()) );
            peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK_PIXEL );
            m_stateFit.setValue<int>( peakKey, (int)(centerPt.y()) );
            fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW_PIXEL );
            m_stateFit.setValue<int>( fbhwKey, (int)(centerPt.x() - offsetPt.x()) );
        }
    }
    m_stateFit.flushState();
}

void Profiler::resetInitialFitGuesses(){
    QString baseKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    int arraySize = m_stateFit.getArraySize( baseKey );
    std::vector<std::tuple<double,double,double> > guesses = _generateFitGuesses( arraySize, false );
    setFitInitialGuesses( guesses );
}

QString Profiler::resetRestFrequency( const QString& curveName ){
    QString result;
    int index = _findCurveIndex( curveName );
    if ( index >= 0 && index < m_plotCurves.size() ){
        m_plotCurves[index]->resetRestFrequency();
        double resetRest = m_plotCurves[index]->getRestFrequency();
        QString resetUnits = m_plotCurves[index]->getRestUnits();
        bool resetType = m_plotCurves[index]->isFrequencyUnits();
        //Store the new rest frequency
        m_state.setValue<double>(CurveData::REST_FREQUENCY, resetRest );
        m_state.setValue<QString>(REST_UNITS, resetUnits );
        m_state.setValue<bool>( CurveData::REST_FREQUENCY_UNITS, resetType );
        m_state.flushState();
        _saveCurveState( index );
        m_stateData.flushState();
        _generateData( m_plotCurves[index]->getLayer(),
                m_plotCurves[index]->getRegion(), false );
    }
    else {
        result = "Could not reset rest frequency, unrecognized profile curve:"+curveName;
    }
    return result;
}

void Profiler::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );
    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_preferences->resetStateString( settingStr );

    //Call methods that require actions if the state value changes.
    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);

    //Reset the rest of the state.
    m_state.setState( prefStr );
    m_state.flushState();
    _updatePlotDisplay();

    m_stateFit.setState( restoredState.getValue<QString>( CurveData::FIT) );
    m_stateFit.flushState();
}


void Profiler::_saveCurveState( int index ){
	int curveCount = m_plotCurves.size();
	int arraySize = m_stateData.getArraySize( CURVES );
	if ( curveCount != arraySize ){
		m_stateData.resizeArray( CURVES, curveCount );
	}
	QString key = Carta::State::UtilState::getLookup( CURVES, index );
	QString curveState = m_plotCurves[index]->getStateString();
	m_stateData.setObject( key, curveState );
}

void Profiler::_saveCurveState(){
    int curveCount = m_plotCurves.size();
    m_stateData.resizeArray( CURVES, curveCount );
    for ( int i = 0; i < curveCount; i++ ){
       _saveCurveState( i );
    }
    m_stateData.flushState();
}

void Profiler::setAutoGenerate( bool autoGenerate ){
	bool oldAutoGenerate = m_state.getValue<bool>( AUTO_GENERATE );
	if ( oldAutoGenerate != autoGenerate ){
		m_state.setValue<bool>( AUTO_GENERATE, autoGenerate );
		m_state.flushState();
		Controller* controller = _getControllerSelected();
		_loadProfile( controller );
	}
}

QString Profiler::setAxisUnitsX( const QString& unitStr ){
    QString result;
    QString actualUnits = m_spectralUnits->getActualUnits( unitStr );
    if ( !actualUnits.isEmpty() ){
        QString oldBottomUnits = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
        if ( actualUnits != oldBottomUnits ){
            //Change the units in the curves.
            int curveCount = m_plotCurves.size();
            for ( int i = 0; i < curveCount; i++ ){
                std::vector<double> converted = _convertUnitsX( m_plotCurves[i], actualUnits );
                m_plotCurves[i]->setDataX( converted );
                if ( m_plotCurves[i]->isFitted() ){
                    converted = _convertUnitsXFit( m_plotCurves[i], actualUnits );
                    m_plotCurves[i]->setDataXFit( converted );
                    converted = _convertUnitsXFitParams( m_plotCurves[i], actualUnits );
                    std::vector< std::tuple<double,double,double> > guesses = m_plotCurves[i]->getFitParams();
                    //Update the centers and fbhws with the converted values.
                    int guessCount = guesses.size();
                    int convertedCount = converted.size();
                    CARTA_ASSERT( convertedCount = guessCount * 2 );
                    for ( int i = 0; i < guessCount; i++ ){
                        std::get<0>(guesses[i])  = converted[2*i];
                        std::get<2>(guesses[i]) = converted[2*i] - converted[2*i+1];
                    }
                    m_plotCurves[i]->setGaussParams( guesses );
                }
            }

            //Update the state & graph
            m_state.setValue<QString>( AXIS_UNITS_BOTTOM, actualUnits);
            m_state.flushState();

            //Set the zoom min & max based on new units
            _updateZoomRangeBasedOnPercent();

            //Tell the plot about the new bounds.
            _updatePlotBounds();

            //Put the data into the plot
            _updatePlotData();
            _updateResidualData();
            _updateFitStatistics();

            //Update channel line
            _updateChannel( _getControllerSelected(), Carta::Lib::AxisInfo::KnownType::SPECTRAL );
        }
    }
    else {
        result = "Unrecognized profile bottom axis units: "+unitStr;
    }
    return result;
}

QString Profiler::setAxisUnitsY( const QString& unitStr ){
    QString result;
    QString actualUnits = m_intensityUnits->getActualUnits( unitStr );
    if ( !actualUnits.isEmpty() ){
        QString oldLeftUnits = m_state.getValue<QString>( AXIS_UNITS_LEFT );
        if ( oldLeftUnits != actualUnits ){
            //Convert the units in the curves.
            int curveCount = m_plotCurves.size();
            for ( int i = 0; i < curveCount; i++ ){
                std::vector<double> converted = _convertUnitsY( m_plotCurves[i], actualUnits );
                m_plotCurves[i]->setDataY( converted );
            }
            //Update the state and plot
            m_state.setValue<QString>( AXIS_UNITS_LEFT, actualUnits );
            m_state.flushState();

            QList<int> unselectedIndices;
            //Go through the curves and find the ones that are fitted.
            //Temporarily select them for fitting keeping track of any indices that were not selected.
            //Generate the fit
            for ( int i = 0; i < curveCount; i++ ){
                if ( m_plotCurves[i]->isFitted() ){
                    if ( !m_plotCurves[i]->isSelectedFit() ){
                        unselectedIndices.append( i );
                        m_plotCurves[i]->setSelectedFit( true );
                    }
                }
            }
            _generateFit();

            //Unselect those that were originally not selected.
            int unselectedCount = unselectedIndices.size();
            for ( int i = 0; i < unselectedCount; i++ ){
                m_plotCurves[ unselectedIndices[i] ]->setSelectedFit( false );
            }
            _updatePlotData();
            _updateResidualData();
            _updateFitStatistics();
        }
    }
    else {
        result = "Unrecognized profile left axis units: "+unitStr;
    }
    return result;
}


QStringList Profiler::setCurveColor( const QString& name, int redAmount, int greenAmount, int blueAmount ){
    QStringList result;
    const int MAX_COLOR = 255;
    bool validColor = true;
    if ( redAmount < 0 || redAmount > MAX_COLOR ){
        validColor = false;
        result.append("Profile curve red amount must be in [0,"+QString::number(MAX_COLOR)+"]: "+QString::number(redAmount) );
    }
    if ( greenAmount < 0 || greenAmount > MAX_COLOR ){
        validColor = false;
        result.append("Profile curve green amount must be in [0,"+QString::number(MAX_COLOR)+"]: "+QString::number(greenAmount) );
    }
    if ( blueAmount < 0 || blueAmount > MAX_COLOR ){
        validColor = false;
        result.append("Profile curve blue amount must be in [0,"+QString::number(MAX_COLOR)+"]: "+QString::number(blueAmount) );
    }
    if ( validColor ){
        int index = _findCurveIndex( name );
        if ( index >= 0 ){
            QColor oldColor = m_plotCurves[index]->getColor();
            QColor curveColor( redAmount, greenAmount, blueAmount );
            if ( oldColor.name() != curveColor.name() ){
                m_plotCurves[index]->setColor( curveColor );
                _saveCurveState( index );
                m_stateData.flushState();
                m_plotManager->setColor( curveColor, name );
            }
        }
        else {
            result.append( "Unrecognized profile curve:"+name );
        }
    }
    return result;
}


QString Profiler::setCurveName( const QString& id, const QString& newName ){
    QString result;
    int curveIndex = _findCurveIndex( id );
    if ( curveIndex >= 0 ){
        result = m_plotCurves[curveIndex]->setName( newName );
        _saveCurveState( curveIndex );
        m_stateData.flushState();
        m_plotManager->setCurveName( id, newName );
        m_state.setValue<QString>( Util::NAME, newName );
        m_state.flushState();
    }
    else {
        result = "Profile name could not be set because of invalid identifier: "+id;
    }
    return result;
}

void Profiler::_setErrorMargin(){
    int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS );
    m_errorMargin = 1.0/qPow(10,significantDigits);
}

QString Profiler::setFitCurves( const QStringList curveNames ){
    QString result;
    int fitCount = curveNames.size();
    //Clear any that were previously selected for fitting.
    int curveCount = m_plotCurves.size();
    bool changed = false;
    for ( int i = 0; i < curveCount; i++ ){
        if ( m_plotCurves[i]->isSelectedFit() ){
            m_plotCurves[i]->setSelectedFit( false );
            changed = true;
        }
    }

    //Set the ones identified selected.
    bool updatedFitState = false;
    for ( int i = 0; i < fitCount; i++ ){
        int curveIndex = _findCurveIndex( curveNames[i]);
        if ( curveIndex >= 0 ){
            changed = true;
            if ( !m_plotCurves[curveIndex]->_isPointSource() ){
                m_plotCurves[curveIndex]->setSelectedFit( true );
                //Update our state with the fit state of the first curve that was selected.
                if ( !updatedFitState && m_plotCurves[curveIndex]->isFitted() ){
                    QString fitState =  m_plotCurves[curveIndex]->getFitState();
                    m_stateFit.setObject( CurveData::FIT, fitState);
                    m_stateFit.flushState();
                    updatedFitState = true;
                }
            }
            else {
                result = "Curve "+m_plotCurves[curveIndex]->getName()+" is a point source and could not be fit.";
            }
        }
        else {
            if ( result.isEmpty() ){
                result = "Could not fit selected curve(s); invalid identifier(s):";
            }
            result = result + curveNames[i];
        }
    }

    if ( changed ){
        _saveCurveState();
    }
    return result;
}

void Profiler::setFitManualGuess( bool manualGuess ){
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, MANUAL_GUESS );
    bool oldManual = m_stateFit.getValue<bool>( key );
    if ( oldManual != manualGuess ){
        m_stateFit.setValue<bool>( key, manualGuess );
        m_stateFit.flushState();
        _generateFit();
    }
}



QString Profiler::setFitInitialGuessesPixels(const std::vector<std::tuple<int,int,int> >& guessPixels ){
    QString result;
    int guessCount = guessPixels.size();
    QString baseKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    for ( int i = 0; i < guessCount; i++ ){
        int centerPixel = std::get<0>( guessPixels[i] );
        int peakPixel = std::get<1>( guessPixels[i] );
        int fbhwPixel = std::get<2>( guessPixels[i] );
        if ( centerPixel < 0 || peakPixel < 0 || fbhwPixel < 0 ){
            QString coord( "("+QString::number(centerPixel)+","+QString::number(peakPixel)+","+
                    QString::number(fbhwPixel)+")");
            result = "Pixel coordinates of initial guesses must be nonnegative integers: " + coord;
            break;
        }
        //Set the pixel coordinates if they have changed.
        QString indexKey = Carta::State::UtilState::getLookup( baseKey, i );
        bool centerChanged = false;
        bool peakChanged = false;
        bool fbhwChanged = false;
        QString centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER_PIXEL );
        int oldCenter = m_stateFit.getValue<int>( centerKey );
        if ( oldCenter != centerPixel ){
            m_stateFit.setValue<int>( centerKey, centerPixel );
            centerChanged = true;
        }
        QString peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK_PIXEL );
        int oldPeak = m_stateFit.getValue<int>( peakKey );
        if ( oldPeak != peakPixel ){
            m_stateFit.setValue<int>( peakKey, peakPixel );
            peakChanged = true;
        }
        QString fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW_PIXEL );
        int oldFBHW = m_stateFit.getValue<int>( fbhwKey );
        if ( oldFBHW != fbhwPixel ){
            m_stateFit.setValue<int>( fbhwKey, fbhwPixel );
            fbhwChanged = true;
        }

        //We recalculate the image image coordinates based on the pixel coordinates.
        bool generate = false;
        bool valid = false;
        QPointF centerPt = m_plotManager->getImagePoint( QPointF(centerPixel,peakPixel), &valid );
        if ( centerChanged || peakChanged ){
            centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER );
            if ( qAbs(m_stateFit.getValue<double>(centerKey) - centerPt.x()) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( centerKey, centerPt.x());
                generate = true;
            }
            peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK );
            if ( qAbs(m_stateFit.getValue<double>(peakKey) - centerPt.y()) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( peakKey, centerPt.y());
                generate = true;
            }
        }
        if ( fbhwChanged ){
            QPointF offsetPt = m_plotManager->getImagePoint(
                    QPointF(centerPixel - fbhwPixel, peakPixel), &valid );
            fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW );
            double fbhwNew = centerPt.x() - offsetPt.x();
            if ( qAbs( m_stateFit.getValue<double>( fbhwKey) - fbhwNew) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( fbhwKey, fbhwNew );
                generate = true;
            }
        }
        if ( centerChanged || peakChanged || fbhwChanged ){
            m_stateFit.flushState();
        }
        if ( generate ){
            _generateFit();
        }
    }
    return result;
}

QString Profiler::setFitInitialGuesses(const std::vector<std::tuple<double,double,double> >& guesses ){
    QString result;
    int guessCount = guesses.size();
    QString baseKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
    int storedArraySize = m_stateFit.getArraySize( baseKey );
    if ( guessCount != storedArraySize ){
        result = "There must be exactly "+QString::number(storedArraySize)+" initial fit guesses.";
    }
    else {
        bool changed = false;
        for ( int i = 0; i < guessCount; i++ ){
            QString indexKey = Carta::State::UtilState::getLookup( baseKey, i );
            QString centerKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_CENTER );
            double oldCenter = m_stateFit.getValue<double>( centerKey );
            double center = std::get<0>( guesses[i] );
            if ( qAbs( oldCenter - center ) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( centerKey, center );
                changed = true;
            }
            QString peakKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_PEAK );
            double oldPeak = m_stateFit.getValue<double>( peakKey );
            double peak = std::get<1>( guesses[i] );
            if ( qAbs( oldPeak - peak ) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( peakKey, peak );
                changed = true;
            }
            QString fbhwKey = Carta::State::UtilState::getLookup( indexKey, CurveData::FIT_FBHW );
            double oldFBHW = m_stateFit.getValue<double>( fbhwKey );
            double fbhw = std::get<2>(guesses[i]);
            if ( qAbs( oldFBHW - fbhw ) > ERROR_MARGIN ){
                m_stateFit.setValue<double>( fbhwKey, fbhw );
                changed = true;
            }
        }
        if ( changed ){
            //Update the pixel coordinates.
            _resetFitGuessPixels();
            _generateFit();
        }
    }
    return result;
}


QString Profiler::setGaussCount( int count ){
    QString result;
    if ( count >= 0 ){
        QString key = Carta::State::UtilState::getLookup( CurveData::FIT, GAUSS_COUNT );
        int oldCount = m_stateFit.getValue<int>( key );
        if ( count != oldCount ){
            m_stateFit.setValue<int>( key, count );

            if ( m_plotCurves.size() > 0 ){
                //Update the initial guess count in the curves.
                _makeInitialGuesses( count );
                //Reset the pixel estimates based on plot size
                _resetFitGuessPixels();

            }
            else {
                //No initial gaussian guesses if there are no curves to fit.
                QString guessKey = Carta::State::UtilState::getLookup( CurveData::FIT, CurveData::INITIAL_GUESSES );
                m_stateFit.resizeArray( guessKey, 0 );
                clearFits();
            }
            if ( count == 0 && getPolyCount() == 0 ){
                clearFits();
            }
            _generateFit();
            m_stateFit.flushState();
        }
    }
    else {
        result = "Profile fit Gaussian count must be nonnegative: "+QString::number( count);
    }
    return result;
}

QString Profiler::setGenerateMode( const QString& modeStr ){
    QString result;
    if ( isAutoGenerate() ){
		QString actualMode = m_generateModes->getActualMode( modeStr );
		if ( !actualMode.isEmpty() ){
			QString oldMode = m_state.getValue<QString>( GEN_MODE );
			if ( actualMode != oldMode ){
				m_state.setValue<QString>( GEN_MODE, actualMode);
				m_state.flushState();
				Controller* controller = _getControllerSelected();
				_loadProfile( controller );
			}
		}
		else {
			result = "Unrecognized profile generation mode: "+modeStr;
		}
    }
    else {
    	result = "Custom profiles can only be set when auto-generation is turned off.";
    }
    return result;
}

void Profiler::setGridLines( bool showLines ){
    bool oldShowLines = m_state.getValue<bool>( GRID_LINES );
    if ( oldShowLines != showLines ){
        m_state.setValue<bool>( GRID_LINES, showLines );
        m_state.flushState();
        m_plotManager->setGridLines( showLines );
    }
}


QString Profiler::setLineStyle( const QString& name, const QString& lineStyle ){
    QString result;
    int index = _findCurveIndex( name );
    if ( index >= 0 ){
        result = m_plotCurves[index]->setLineStyle( lineStyle );
        if ( result.isEmpty() ){
            _saveCurveState( index );
            m_stateData.flushState();
            QString actualStyle = m_lineStyles->getActualLineStyle( lineStyle );
            m_plotManager->setLineStyle( actualStyle, name );
        }
    }
    else {
        result = "Profile curve was not recognized: "+name;
    }
    return result;
}

QString Profiler::setLineStyleFit( const QString& lineStyleFit ){
    QString result;
    int curveCount = m_plotCurves.size();
    QString actualStyle = m_lineStyles->getActualLineStyle( lineStyleFit );
    if ( !actualStyle.isEmpty()){
        for ( int index = 0; index < curveCount; index ++ ){
            if ( m_plotCurves[index]->isSelectedFit()){
                result = m_plotCurves[index]->setLineStyleFit( lineStyleFit );
                if ( !result.isEmpty() ){
                    break;
                }
                else {
                    m_plotManager->setLineStyle( actualStyle, m_plotCurves[index]->getName(),0, false );
                }
            }
        }
        if ( result.isEmpty()){
            _saveCurveState(  );
            m_stateData.flushState();
            m_state.setValue<QString>(CurveData::STYLE_FIT, actualStyle );
            m_state.flushState();

        }
    }
    else {
        result = "Line style for fit curve not recognized: "+lineStyleFit;
    }
    return result;
}

QString Profiler::setLegendLocation( const QString& locateStr ){
    QString result;
    QString actualLocation = m_legendLocations->getActualLocation( locateStr );
    if ( !actualLocation.isEmpty() ){
        QString oldLocation = m_state.getValue<QString>( LEGEND_LOCATION );
        if ( oldLocation != actualLocation ){
            m_state.setValue<QString>( LEGEND_LOCATION, actualLocation );
            m_state.flushState();
            m_plotManager->setLegendLocation( actualLocation );
        }
    }
    else {
        result = "Unrecognized profile legend location: "+locateStr;
    }
    return result;
}

void Profiler::setLegendExternal( bool external ){
    bool oldExternal = m_state.getValue<bool>( LEGEND_EXTERNAL );
    if ( external != oldExternal ){
        m_state.setValue<bool>( LEGEND_EXTERNAL, external );
        m_legendLocations->setAvailableLocations(external);
        //Check to see if the current location is still supported.  If not,
        //use the default.
        QString currPos = m_state.getValue<QString>( LEGEND_LOCATION );
        QString actualPos = m_legendLocations->getActualLocation( currPos );
        if ( actualPos.isEmpty() ){
            QString newPos = m_legendLocations->getDefaultLocation( external );
            m_state.setValue<QString>( LEGEND_LOCATION, newPos );
            m_plotManager->setLegendLocation( newPos );
        }
        m_state.flushState();

        m_plotManager->setLegendExternal( external );
    }
}

void Profiler::setLegendLine( bool showLegendLine ){
    bool oldShowLine = m_state.getValue<bool>( LEGEND_LINE );
    if ( oldShowLine != showLegendLine ){
        m_state.setValue<bool>(LEGEND_LINE, showLegendLine );
        m_state.flushState();
        m_plotManager->setLegendLine( showLegendLine );
    }
}

void Profiler::setLegendShow( bool showLegend ){
    bool oldShowLegend = m_state.getValue<bool>( LEGEND_SHOW );
    if ( oldShowLegend != showLegend ){
        m_state.setValue<bool>(LEGEND_SHOW, showLegend );
        m_state.flushState();
        m_plotManager->setLegendShow( showLegend );
    }
}

QString Profiler::setPlotStyle( const QString& name, const QString& plotStyle ){
    QString result;
    int index = _findCurveIndex( name );
    if ( index >= 0 ){
        result = m_plotCurves[index]->setPlotStyle( plotStyle );
        if ( result.isEmpty() ){
            _saveCurveState( index );
            m_stateData.flushState();
            ProfilePlotStyles* plotStyles = Util::findSingletonObject<ProfilePlotStyles>();
            QString actualStyle = plotStyles->getActualStyle( plotStyle );
            m_plotManager->setStyle( actualStyle, name );
        }
    }
    else {
        result = "Profile curve was not recognized: "+name;
    }
    return result;
}

QString Profiler::setPolyCount( int count ){
    QString result;
    if ( count >= 0 ){
        QString key = Carta::State::UtilState::getLookup( CurveData::FIT, POLY_DEGREE );
        int oldCount = m_stateFit.getValue<int>( key );
        if ( count != oldCount ){
            m_stateFit.setValue<int>( key, count );
            m_stateFit.flushState();
            _generateFit();
            if ( count == 0 && getGaussCount() == 0 ){
                clearFits();
            }
        }
    }
    else {
        result = "Profile fit polynomial count must be nonnegative: "+QString::number( count);
    }
    return result;
}

void Profiler::setRandomHeuristics( bool randomHeuristics ){
    QString key = Carta::State::UtilState::getLookup( CurveData::FIT, HEURISTICS );
    bool oldRandom = m_stateFit.getValue<bool>( key );
    if ( randomHeuristics != oldRandom ){
        m_stateFit.setValue<bool>( key, randomHeuristics );
        m_stateFit.flushState();
        _generateFit();
    }
}

QString Profiler::setRestFrequency( double freq, const QString& curveName ){
    QString result;
    if ( freq >= 0 ){
		int index = _findCurveIndex( curveName );
		int significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
		double roundedFreq = Util::roundToDigits( freq, significantDigits );
		if ( index >= 0 ){
			bool freqSet = false;
			result = m_plotCurves[index]->setRestFrequency( roundedFreq, m_errorMargin, &freqSet );
			if ( freqSet ){
				_saveCurveState( index );
				m_stateData.flushState();
				_generateData( m_plotCurves[index]->getLayer(), m_plotCurves[index]->getRegion(), false );
			}
		}

		//We store the frequency, assuming the user will be using it to create a new
		//profile.
		m_state.setValue<double>(CurveData::REST_FREQUENCY, roundedFreq );
		m_state.flushState();
    }
    else {
    	result = "Frequency must be nonnegative.";
    }
    return result;
}

QString Profiler::setRestUnits( const QString& restUnits, const QString& curveName ){
	QString result;
	bool freqUnits = m_state.getValue<bool>( CurveData::REST_FREQUENCY_UNITS );

	QString actualUnits;
	if ( freqUnits ){
		actualUnits = m_frequencyUnits->getActualUnits( restUnits );
	}
	else {
		actualUnits = m_wavelengthUnits ->getActualUnits( restUnits );
	}
	if ( !actualUnits.isEmpty() ){
		int index = _findCurveIndex( curveName );
		if ( index >= 0 ){
			int signDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
			bool restUnitsChanged = m_plotCurves[index]->setRestUnits( actualUnits, signDigits, m_errorMargin );
			if ( restUnitsChanged ){
				_saveCurveState( index );
				m_stateData.flushState();
			}
		}
		m_state.setValue<QString>(REST_UNITS, actualUnits );
		m_state.flushState();
	}
	else {
		result = "Unrecognized rest units: " + restUnits;
	}

    return result;
}


QString Profiler::setRestUnitType( bool restUnitsFreq, const QString& curveName ){
    QString result;
    int index = _findCurveIndex( curveName );
    if ( index >= 0 ){
        int signDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
        bool changed = m_plotCurves[index]->setRestUnitType( restUnitsFreq, signDigits, m_errorMargin );
        if ( changed ){
        	double restFreq = m_plotCurves[index]->getRestFrequency();
        	QString restUnits = m_plotCurves[index]->getRestUnits();
        	if ( getRestFrequency("") != restFreq ){
        		m_state.setValue<double>(CurveData::REST_FREQUENCY, restFreq );
        		m_state.setValue<QString>(REST_UNITS, restUnits );
        	}
        	_saveCurveState( index );
        	m_stateData.flushState();
        }
    }
    if ( m_state.getValue<bool>(CurveData::REST_FREQUENCY_UNITS) != restUnitsFreq ){
    	m_state.setValue<bool>( CurveData::REST_FREQUENCY_UNITS, restUnitsFreq );
    	 m_state.flushState();
    }

    return result;
}

QString Profiler::setSelectedCurve( const QString& curveId ){
    QString result;
    if ( curveId.trimmed().length() > 0 ){
        //Make sure this is really an existing curve.
        int curveIndex = _findCurveIndex( curveId );
        if ( curveIndex >= 0 ){
            QString oldCurveName = m_stateData.getValue<QString>( CURVE_SELECT );
            if ( oldCurveName != curveId ){
                m_stateData.setValue<QString>( CURVE_SELECT, curveId );
                m_stateData.flushState();
                //Region and image select should match curve selected.
                std::shared_ptr<Region> region = m_plotCurves[curveIndex]->getRegion();
                if ( region ){
                    setSelectedRegion( region->getRegionName() );
                }
                std::shared_ptr<Layer> layer = m_plotCurves[curveIndex]->getLayer();
                if ( layer ){
                    setSelectedLayer( layer->_getLayerName() );
                }
            }
        }
        else {
            result = "Could not find curve to select: "+curveId;
        }
    }
    else {
        result = "Please specify a curve to select.";
    }
    return result;
}


QString Profiler::setSelectedLayer( const QString& imageId ){
    QString result;
    if ( imageId.trimmed().length() > 0 ){
        //Make sure this is really an existing image.
        Controller* selectedController = _getControllerSelected();
        bool layerFound = false;
        std::pair<double,QString> restFreq(-1,"");
        if ( selectedController ){
            std::vector<std::shared_ptr<Layer> > layers = selectedController->getLayers();
            for ( std::shared_ptr<Layer> layer : layers ){
                QString layerName = layer->_getLayerName();

                if ( layerName == imageId ){
                	restFreq = layer->_getRestFrequency();
                    layerFound = true;
                    break;
                }
            }
        }
        if ( layerFound ){
            QString oldImageName = m_stateData.getValue<QString>( IMAGE_SELECT );
            if ( oldImageName != imageId ){
            	//Update the rest frequency to reflect that of the image
            	setRestFrequency( restFreq.first, imageId );
            	setRestUnits( restFreq.second, imageId );
                m_stateData.setValue<QString>( IMAGE_SELECT, imageId );
                m_stateData.flushState();
            }
        }
        else {
            result = "Could not find image to select: "+imageId;
        }
    }
    else {
        result = "Please specify an image to select.";
    }
    return result;
}


QString Profiler::setSelectedRegion( const QString& regionName ){
    QString result;
    if ( regionName.trimmed().length() > 0 ){
        bool regionFound = false;
        if ( regionName == NO_REGION ){
            regionFound = true;
        }
        else {
            //Make sure this is really an existing image.
            Controller* selectedController = _getControllerSelected();
            if ( selectedController ){
            	std::shared_ptr<RegionControls> regionControls = selectedController->getRegionControls();
				if ( regionControls ){
					std::shared_ptr<Region> region = regionControls->getRegion( regionName );
					if ( region ){
						regionFound = true;
					}
				}
            }
        }

        if ( regionFound ){
            QString oldRegion = m_stateData.getValue<QString>( REGION_SELECT );
            if ( oldRegion != regionName ){
                m_stateData.setValue<QString>( REGION_SELECT, regionName );
                m_stateData.flushState();
            }
        }
        else {
            result = "Could not find region to select: "+regionName;
        }
    }
    else {
        result = "Please specify a region to select.";
    }
    return result;
}

QString Profiler::setShowFitGuesses( bool showFitGuesses ){
    QString result;
    bool oldShow = m_state.getValue<bool>( SHOW_GUESSES );
    if ( oldShow != showFitGuesses ){
        //We will only show them if we are in manual mode.
        if ( isFitManualGuess() ){
            m_state.setValue<bool>(SHOW_GUESSES, showFitGuesses );
            m_state.flushState();
        }
        else {
            result = "Fit guesses will only be displayed when manual guesses are specified.";
        }
    }
    return result;
}

void Profiler::setShowFitResiduals( bool showFitResiduals ){
    bool oldShowResiduals = m_state.getValue<bool>( SHOW_RESIDUALS );
    if ( oldShowResiduals != showFitResiduals ){
        m_state.setValue<bool>(SHOW_RESIDUALS, showFitResiduals );
        m_state.flushState();
        if ( showFitResiduals ){
            m_residualPlotIndex = m_plotManager->addPlot();
           _updateResidualData();
        }
        else {
            if ( m_residualPlotIndex >= 0 ){
                m_plotManager->removePlot(m_residualPlotIndex);
                m_residualPlotIndex = -1;
            }
        }
    }
}

void Profiler::setShowCursor( bool showCursor ){
    bool oldShow = m_state.getValue<bool>( SHOW_CURSOR );
    if ( oldShow != showCursor ){
        m_state.setValue<bool>(SHOW_CURSOR, showCursor );
        m_state.flushState();
        m_plotManager->setCursorEnabled( showCursor );
    }
}

void Profiler::setShowFitStatistics( bool showFitStatistics ){
    bool oldShow = m_state.getValue<bool>( SHOW_STATISTICS );
    if ( oldShow != showFitStatistics ){
        m_state.setValue<bool>(SHOW_STATISTICS, showFitStatistics );
        m_state.flushState();
        if ( !showFitStatistics ){
            //If there are now curves that are fitted, blank the statistics.
            int curveCount = m_plotCurves.size();
            bool fitsExist = false;
            for ( int i = 0; i < curveCount; i++ ){
                if ( m_plotCurves[i]->isFitted()){
                    fitsExist = true;
                    break;
                }
            }
            if ( !fitsExist ){
                m_stateFitStatistics.setValue<QString>( FIT_STATISTICS, "");
                m_stateFitStatistics.flushState();
            }
        }
    }
}

void Profiler::setShowFrame( bool showFrame ){
    bool oldShow = m_state.getValue<bool>( SHOW_FRAME );
    if ( oldShow != showFrame ){
        m_state.setValue<bool>(SHOW_FRAME, showFrame );
        m_state.flushState();
        m_plotManager->setVLineVisible( showFrame );
    }
}

void Profiler::setShowMeanRMS( bool showMeanRMS ){
    bool oldShow = m_state.getValue<bool>( SHOW_MEAN_RMS );
    if ( oldShow != showMeanRMS ){
        m_state.setValue<bool>(SHOW_MEAN_RMS, showMeanRMS );
        m_state.flushState();
        m_plotManager->setRangeMarkerVisible( showMeanRMS );
        m_plotManager->setHLineVisible( showMeanRMS );
        m_plotManager->updatePlot();
    }
}

void Profiler::setShowPeakLabels( bool showPeakLabels ){
    bool oldShow = m_state.getValue<bool>( SHOW_PEAK_LABELS );
    if ( oldShow != showPeakLabels ){
        m_state.setValue<bool>(SHOW_PEAK_LABELS, showPeakLabels );
        m_state.flushState();
        _updatePlotData();
    }
}

QString Profiler::setSignificantDigits( int digits ){
    QString result;
    if ( digits <= 0 ){
        result = "Invalid significant digits; must be positive:  "+QString::number( digits );
    }
    else {
        if ( m_state.getValue<int>(Util::SIGNIFICANT_DIGITS) != digits ){
            m_state.setValue<int>(Util::SIGNIFICANT_DIGITS, digits );
            _setErrorMargin();
        }
    }
    return result;
}

QString Profiler::setStatistic( const QString& statStr, const QString& curveName ){
    QString result;
    Carta::Lib::ProfileInfo::AggregateType agType = m_stats->getTypeFor( statStr );
    if ( agType != Carta::Lib::ProfileInfo::AggregateType::OTHER ){
		int index = _findCurveIndex( curveName );
		if ( index >= 0 ){
			result = m_plotCurves[index]->setStatistic( statStr );
			if ( result.isEmpty() ){
				_saveCurveState( index );
				m_stateData.flushState();
				m_intensityUnits->resetUnits( agType );
				QString unitDefault = m_intensityUnits->getDefault();
				setAxisUnitsY( unitDefault );
				_generateData( m_plotCurves[index]->getLayer(), m_plotCurves[index]->getRegion(), false );
			}
		}
		m_state.setValue<QString>(CurveData::STATISTIC, statStr );
    }
    else {
    	result = "Unknown profile aggregate type.";
    }
    return result;
}

QString Profiler::setTabIndex( int index ){
    QString result;
    if ( index >= 0 ){
        int oldIndex = m_state.getValue<int>( Util::TAB_INDEX );
        if ( index != oldIndex ){
            m_state.setValue<int>( Util::TAB_INDEX, index );
            m_state.flushState();
        }
    }
    else {
        result = "Profile tab index must be nonnegative: "+ QString::number(index);
    }
    return result;
}


void Profiler::setZoomBuffer( bool zoomBuffer ){
    bool oldZoomBuffer = m_stateData.getValue<bool>( ZOOM_BUFFER );
    if ( oldZoomBuffer != zoomBuffer ){
        m_stateData.setValue<bool>( ZOOM_BUFFER, zoomBuffer );
        m_stateData.flushState();
        _updatePlotBounds();
    }

}

QString Profiler::setZoomBufferSize( double zoomBufferSize ){
    QString result;
    if ( zoomBufferSize >= 0 && zoomBufferSize < 100 ){
        double oldBufferSize = m_stateData.getValue<double>( ZOOM_BUFFER_SIZE );
        int significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
        double roundedSize = Util::roundToDigits( zoomBufferSize, significantDigits );
        if ( qAbs( roundedSize - oldBufferSize) > m_errorMargin ){
            m_stateData.setValue<double>( ZOOM_BUFFER_SIZE, roundedSize );
            m_stateData.flushState();
            _updatePlotBounds();
        }
    }
    else {
        result = "Zoom buffer size must be in [0,100): "+QString::number(zoomBufferSize);
    }
    return result;
}


QString Profiler::setZoomRange( double zoomMin, double zoomMax ){
    QString result;
    double significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
    double zoomMinRounded = Util::roundToDigits( zoomMin, significantDigits );
    double zoomMaxRounded = Util::roundToDigits( zoomMax, significantDigits );
    if ( zoomMinRounded < zoomMaxRounded ){
        bool changed = false;
        double oldZoomMin = m_stateData.getValue<double>( ZOOM_MIN );
        if ( qAbs( zoomMinRounded - oldZoomMin ) > m_errorMargin ){
            changed = true;
            m_stateData.setValue<double>( ZOOM_MIN, zoomMinRounded );
        }
        double oldZoomMax = m_stateData.getValue<double>( ZOOM_MAX );
        if ( qAbs( zoomMaxRounded - oldZoomMax ) > m_errorMargin ){
            changed = true;
            m_stateData.setValue<double>( ZOOM_MAX, zoomMaxRounded );
        }
        if ( changed ){
            //Update the percents to match.
            std::pair<double,double> curveRange = _getCurveRangeX();

            double lowerPercent = 0;
            double upperPercent = 100;
            double curveSpan = curveRange.second - curveRange.first;
            if ( curveSpan > 0 ){
                if ( curveRange.first < zoomMinRounded ){
                    double diff = zoomMinRounded - curveRange.first;
                    lowerPercent = diff / curveSpan * 100;
                    lowerPercent = Util::roundToDigits( lowerPercent, significantDigits );
                }
                if ( curveRange.second > zoomMaxRounded ){
                    double diff = curveRange.second - zoomMaxRounded;
                    upperPercent = 100 - diff / curveSpan * 100;
                    upperPercent = Util::roundToDigits( upperPercent, significantDigits );
                }
            }
            m_stateData.setValue<double>( ZOOM_MIN_PERCENT, lowerPercent );
            m_stateData.setValue<double>( ZOOM_MAX_PERCENT, upperPercent );
            m_stateData.flushState();

           _updatePlotBounds();
        }
    }
    else {
        result = "Minimum zoom, "+QString::number(zoomMin)+", must be less the maximum zoom, "+QString::number(zoomMax);
    }
    return result;
}


QString Profiler::setZoomRangePercent( double zoomMinPercent, double zoomMaxPercent ){
    QString result;

    if ( 0 <= zoomMinPercent && zoomMinPercent <= 100 ){
        if ( 0 <= zoomMaxPercent && zoomMaxPercent <= 100 ){
            int significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
            double zoomMinPercentRounded = Util::roundToDigits( zoomMinPercent, significantDigits );
            double zoomMaxPercentRounded = Util::roundToDigits( zoomMaxPercent, significantDigits );
            if ( zoomMinPercentRounded < zoomMaxPercentRounded ){
                bool changed = false;
                double oldZoomMinPercent = m_stateData.getValue<double>( ZOOM_MIN_PERCENT );
                if ( qAbs( zoomMinPercentRounded - oldZoomMinPercent ) > m_errorMargin ){
                    changed = true;
                    m_stateData.setValue<double>( ZOOM_MIN_PERCENT, zoomMinPercentRounded );
                }
                double oldZoomMaxPercent = m_stateData.getValue<double>( ZOOM_MAX_PERCENT );
                if ( qAbs( zoomMaxPercentRounded - oldZoomMaxPercent ) > m_errorMargin ){
                    changed = true;
                    m_stateData.setValue<double>( ZOOM_MAX_PERCENT, zoomMaxPercentRounded );
                }
                if ( changed ){
                    //Update the zoom min and max.
                    _updateZoomRangeBasedOnPercent();
                    //Update the graph.
                    _updatePlotBounds();
                }
            }
            else {
                result = "Zoom minimum percent: "+ QString::number(zoomMinPercent)+" must be less than "+QString::number( zoomMaxPercent);
            }
        }
        else {
            result = "Invalid zoom right percent [0,100]: "+QString::number(zoomMaxPercent);
        }
    }
    else {
        result = "Invalid zoom left percent [0,100]: "+QString::number( zoomMinPercent);
    }
    return result;
}



void Profiler::timerEvent( QTimerEvent* /*event*/ ){
    Controller* controller = _getControllerSelected();
    if ( controller ){
        controller->_setFrameAxis( m_oldFrame, Carta::Lib::AxisInfo::KnownType::SPECTRAL );
        _updateChannel( controller, Carta::Lib::AxisInfo::KnownType::SPECTRAL );
        if ( m_oldFrame < m_currentFrame ){
            m_oldFrame++;
        }
        else if ( m_oldFrame > m_currentFrame ){
            m_oldFrame--;
        }
        else {
            killTimer(m_timerId );
            m_timerId = 0;
        }
    }
}


void Profiler::_updateChannel( Controller* controller, Carta::Lib::AxisInfo::KnownType type ){
    if ( type == Carta::Lib::AxisInfo::KnownType::SPECTRAL ){
        int frame = controller->getFrame( type );
        //Convert the frame to the units the plot is using.
        QString bottomUnits = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
        QString units = _getUnitUnits( bottomUnits );
        std::vector<double> values(1);
        values[0] = frame;
        if ( m_plotCurves.size() > 0 ){
            if ( !units.isEmpty() ){
                std::shared_ptr<Layer> layer = m_plotCurves[0]->getLayer();
                _convertX(  values, layer, "", units );
            }
            m_plotManager->setVLinePosition( values[0] );
        }
    }
}

void Profiler::_updateFitStatistics(  ){
    int resultCount = m_plotCurves.size();
    QString stats;
    for ( int i = 0; i < resultCount; i++ ){
        if ( m_plotCurves[i]->isSelectedFit() && m_plotCurves[i]->isFitted()){
            stats = stats + _updateFitStatistic( i );
        }
    }
    m_stateFitStatistics.setValue<QString>( FIT_STATISTICS, stats );
    m_stateFitStatistics.flushState();
}

QString Profiler::_updateFitStatistic( int curveIndex ){
    QStringList fitStats;
    if ( curveIndex >= 0 ){
        const QString START_BOLD="<span style='font-weight:bold'>";
        const QString END_BOLD="</span>";
        fitStats.append( START_BOLD + m_plotCurves[curveIndex]->getName()
                +": "+ END_BOLD + m_plotCurves[curveIndex]->getFitStatus() );
        double rms = m_plotCurves[curveIndex]->getFitRMS();
        fitStats.append( START_BOLD +"  RMS: "+END_BOLD+QString::number(rms) );
        fitStats.append( START_BOLD + " Data Count: "+ END_BOLD + QString::number( m_plotCurves[curveIndex]->getDataCount() )+"<br/>");

        std::vector<std::tuple<double,double,double> > gaussFits =
                m_plotCurves[curveIndex]->getGaussParams();
        int gaussCount = gaussFits.size();
        bool manualGuess = isFitManualGuess();

        QString axisXUnits = getAxisUnitsX();
        QString axisYUnits = getAxisUnitsY();
        QString PIX( "pix");
        QString xUnits = this->_getUnitUnits( axisXUnits );
        if ( gaussCount > 0 ){
            fitStats.append( "<table>");
            fitStats.append( "<tr><th>Gaussian</th><th>Result:</th><th>Center(" );
            if( xUnits.length() > 0 ){
                fitStats.append( xUnits );
            }
            else {
                fitStats.append( PIX );
            }
            fitStats.append(")</th><th>Peak("+axisYUnits+")</th><th>FWHM(");
            if ( xUnits.length() > 0 ){
                fitStats.append( xUnits );
            }
            else {
                fitStats.append( PIX );
            }
            fitStats.append( ")</th>");
            if ( manualGuess ){
                fitStats.append( "<th>Initial:</th><th>Center(");
                if ( xUnits.length() > 0 ){
                    fitStats.append( xUnits );
                }
                else {
                    fitStats.append( PIX );
                }
                fitStats.append(")</th><th>Peak("+axisYUnits+")</th><th>FWHM(" );
                if ( xUnits.length() > 0 ){
                    fitStats.append( xUnits );
                }
                else {
                    fitStats.append( PIX );
                }
                fitStats.append( ")</th>");
            }
            fitStats.append( "</tr>");
        }

        for ( int i = 0; i < gaussCount; i++ ){
            double centerXPix = std::get<0>(gaussFits[i]);
            std::vector<double> centerVect(1);
            centerVect[0] = centerXPix;
            if ( xUnits.length() > 0 ){
                _convertX( centerVect, m_plotCurves[curveIndex]->getLayer(), "", xUnits );
            }

            fitStats.append( "<tr>");
            fitStats.append( "<td>" + QString::number(i+1) + "</td>" );
            fitStats.append( "<td></td>");
            fitStats.append( "<td>" + QString::number( centerVect[0] ) );
            fitStats.append( "</td>");

            fitStats.append( "<td>" + QString::number(std::get<1>(gaussFits[i])) + "</td>");

            double fbhwPix = std::get<2>(gaussFits[i]);
            std::vector<double> leftVect(1);
            leftVect[0] = centerXPix - fbhwPix;
            if ( xUnits.length() > 0 ){
                _convertX( leftVect, m_plotCurves[curveIndex]->getLayer(), "", xUnits );
                leftVect[0] = centerVect[0] - leftVect[0];
            }
            fitStats.append( "<td>" + QString::number( leftVect[0] ) + "</td>" );
            if ( manualGuess ){
                double center = m_plotCurves[curveIndex]->getFitParamCenter( i );
                double peak = m_plotCurves[curveIndex]->getFitParamPeak( i );
                double fbhw = m_plotCurves[curveIndex]->getFitParamFBHW( i );
                fitStats.append( "<td></td>");
                fitStats.append( "<td>" + QString::number( center ) + "</td>" );
                fitStats.append( "<td>" + QString::number(peak) + "</td>");
                fitStats.append( "<td>" + QString::number(fbhw) + "</td>");
            }
            fitStats.append( "</tr>");
        }
        if ( gaussCount > 0 ){
            fitStats.append( "</table>");
        }
        std::vector<double> polyTerms = m_plotCurves[curveIndex]->getFitPolyCoeffs();
        int termCount = polyTerms.size();
        if ( termCount > 0 ){
            QString polyTermsStr( START_BOLD + "Polynomial:"+END_BOLD);
            QString x( " x");
            for ( int i = 0; i < termCount; i++ ){
                polyTermsStr = polyTermsStr + QString::number(polyTerms[i]);
                if ( i > 0 ){
                    QString powerStr = x;
                    if ( i > 1 ){
                        powerStr = powerStr + "^"+QString::number(i);
                    }
                    polyTermsStr = polyTermsStr + powerStr;
                }
                if ( i < termCount - 1 ){
                    polyTermsStr = polyTermsStr + " + ";
                }
            }
            fitStats.append( polyTermsStr );
        }
    }
    return fitStats.join("");
}

void Profiler::_updatePlotBounds(){
    //Update the graph.
    //See if we need to add an additional buffer.
    double graphMin = m_stateData.getValue<double>( ZOOM_MIN );
    double graphMax = m_stateData.getValue<double>( ZOOM_MAX );
    double plotRange = graphMax - graphMin;
    if ( m_stateData.getValue<bool>( ZOOM_BUFFER) ){
        double bufferSize = m_stateData.getValue<double>( ZOOM_BUFFER_SIZE );
        double halfSize = bufferSize / 2;
        double buffAmount = plotRange * halfSize / 100;
        graphMin = graphMin - buffAmount;
        graphMax = graphMax + buffAmount;
    }
    m_plotManager->setAxisXRange( graphMin, graphMax );
    _resetFitGuessPixels();
}

void Profiler::_updatePlotDisplay(){
    if ( m_plotManager ){
        bool showMeanRMS = m_state.getValue<bool>( SHOW_MEAN_RMS );
        m_plotManager->setRangeMarkerVisible( showMeanRMS );
        m_plotManager->setHLineVisible( showMeanRMS );
        bool showResiduals = m_state.getValue<bool>( SHOW_RESIDUALS );
        if ( showResiduals ){
            m_residualPlotIndex = m_plotManager->addPlot();
        }
        else {
            if ( m_residualPlotIndex >= 0 ){
                m_plotManager->removePlot( m_residualPlotIndex );
                m_residualPlotIndex = -1;
            }
        }
        m_plotManager->setVLineVisible( m_state.getValue<bool>(SHOW_FRAME) );
        m_plotManager->setLegendLocation(m_state.getValue<QString>( LEGEND_LOCATION));
        m_plotManager->setLegendExternal( m_state.getValue<bool>( LEGEND_EXTERNAL ));
        m_plotManager->setLegendShow( m_state.getValue<bool>( LEGEND_SHOW ));
        m_plotManager->setLegendLine( m_state.getValue<bool>( LEGEND_LINE ));
        m_plotManager->setCursorEnabled( m_state.getValue<bool>( SHOW_CURSOR ) );
    }
}

void Profiler::_updateResidualData(){
    if ( isShowResiduals() ){
        int curveCount = m_plotCurves.size();
        for ( int i = 0; i < curveCount; i++ ){
            if ( m_plotCurves[i]->isFitted() ){
                std::vector< std::pair<double,double> > res =
                        m_plotCurves[i]->getFitResiduals();
                QString resName = m_plotCurves[i]->getName();
                Carta::Lib::Hooks::Plot2DResult* resRes = new Carta::Lib::Hooks::Plot2DResult( resName,
                        "", "", res );
                m_plotManager->addData( resRes, m_residualPlotIndex );
            }
        }
        QString leftUnit = m_state.getValue<QString>( AXIS_UNITS_LEFT );
        m_plotManager->setTitleAxisY( leftUnit );
    }
}

void Profiler::_updateSelectedCurve(){
	if ( isAutoGenerate() ){
		int curveCount = m_plotCurves.size();
		if ( curveCount == 0 ){
			m_stateData.setValue<QString>( CURVE_SELECT, "");
		}
		else {
			//Choose the curve with largest index that is active and matches
			//the current region and image.
			for ( int i = curveCount - 1; i >= 0; i-- ){
				if ( m_plotCurves[i]->isActive() ){
					 QString selectedName = m_plotCurves[i]->getName();
					 setSelectedCurve( selectedName );
				}
			}
		}
	}
}

void Profiler::_updateZoomRangeBasedOnPercent(){
    std::pair<double,double> range = _getCurveRangeX();
    double curveSpan = range.second - range.first;
    double minPercent = getZoomMinPercent();
    double maxPercent = getZoomMaxPercent();
    double zoomMin = range.first + minPercent* curveSpan / 100;
    double zoomMax = range.second -(100 - maxPercent)* curveSpan / 100;
    int significantDigits = m_state.getValue<int>( Util::SIGNIFICANT_DIGITS );
    zoomMin = Util::roundToDigits( zoomMin, significantDigits );
    zoomMax = Util::roundToDigits( zoomMax, significantDigits );
    double oldZoomMin = getZoomMin();
    double oldZoomMax = getZoomMax();
    bool changed = false;
    if ( qAbs( oldZoomMin - zoomMin ) > m_errorMargin ){
        m_stateData.setValue<double>( ZOOM_MIN, zoomMin );
        changed = true;
    }
    if ( qAbs( oldZoomMax - zoomMax ) > m_errorMargin ){
        m_stateData.setValue<double>( ZOOM_MAX, zoomMax );
        changed = true;
    }
    if ( changed ){
        m_stateData.flushState();
    }
}


void Profiler::_updatePlotData(){
    int curveCount = m_plotCurves.size();
    m_plotManager->clearLabels();

    //Put the data into the plot.
    for ( int i = 0; i < curveCount; i++ ){
    	if ( m_plotCurves[i]->isActive() ){
			std::vector< std::pair<double,double> > plotData = m_plotCurves[i]->getPlotData();
			QString dataId = m_plotCurves[i]->getName();
			Carta::Lib::Hooks::Plot2DResult plotResult( dataId, "", "", plotData );
			m_plotManager->addData( &plotResult );
			bool fitted = m_plotCurves[i]->isFitted();
			if ( fitted ){
				std::vector< std::pair<double,double> > fitData = m_plotCurves[i]->getFitData();
				Carta::Lib::Hooks::Plot2DResult fitResult( dataId, "", "", fitData );

				m_plotManager->addData( &fitResult, 0, false );
				m_plotManager->setLineStyle( m_plotCurves[i]->getLineStyleFit(),
						dataId, 0, false);
				QString xUnits = getAxisUnitsX();
				QString yUnits = getAxisUnitsY();
				if ( isShowPeakLabels() ){
					std::vector< std::tuple<double,double,QString> > peakLabels = m_plotCurves[i]->getPeakLabels(xUnits, yUnits );
					m_plotManager->addLabels( peakLabels );
				}
			}
			m_plotManager->setColor( m_plotCurves[i]->getColor(), dataId );
    	}
    }

    QString bottomUnit = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
    bottomUnit = _getUnitUnits( bottomUnit );
    QString leftUnit = m_state.getValue<QString>( AXIS_UNITS_LEFT );
    m_plotManager->setTitleAxisX( bottomUnit );
    m_plotManager->setTitleAxisY( leftUnit );
    m_plotManager->updatePlot();
}

bool Profiler::_updateProfiles( Controller* controller ){
    bool profileChanged = false;

    if ( isAutoGenerate() ){
		//Go through the old profiles and set inactive any whose regions&layers are not in
    	//the prescribed list.
    	_removeUnsupportedCurves();
		int curveCount = m_plotCurves.size();
		std::vector<std::shared_ptr<Layer> > layers = _getDataForGenerateMode( controller );
		std::vector<std::shared_ptr<Region> > regions = _getRegionForGenerateMode( /*controller*/ );
		int dataCount = layers.size();
		int regionCount = regions.size();
		bool curveStatusChange = false;
		for ( int i = 0; i < curveCount; i++ ){
			QString curveId = m_plotCurves[i]->getName();
			bool layerFound = false;
			for ( int j = 0; j < dataCount; j++ ){
				if ( regionCount > 0 ){
					for ( int k = 0; k < regionCount; k++ ){
						QString id = CurveData::_generateName( layers[j], regions[k] );
						if ( id == curveId ){
							layerFound = true;
							break;
						}
					}
					if ( layerFound ){
						break;
					}
				}
				else {
					QString id = CurveData::_generateName( layers[j], nullptr );
					if ( id == curveId ){
						layerFound = true;
						break;
					}
				}
			}
			bool activeChanged = m_plotCurves[i]->setActive( layerFound );
			if ( activeChanged ){
				curveStatusChange = true;
			}
		}

		//Make profiles for any new data that has been loaded.
		for ( int i = 0; i < dataCount; i++ ) {
			if ( regionCount > 0 ){
				for ( int j = 0; j < regionCount; j++ ){
					bool curveGenerated = _generateCurve( layers[i], regions[j] );
					if ( curveGenerated ){
						profileChanged = true;
					}
				}
			}
			else {
				//Profile of entire image
				bool curveGenerated = _generateCurve( layers[i], nullptr );
				if ( curveGenerated ){
					profileChanged = true;
				}
			}
		}

		_saveCurveState();
		if ( curveStatusChange && !profileChanged ){
			_updateSelectedCurve();
		}
    }
    return profileChanged;
}

QString Profiler::_zoomToSelection(){
    QString result;
    bool valid = false;
    std::pair<double,double> range = m_plotManager->getRange( & valid );
    if ( valid ){
        double minRange = range.first;
        double maxRange = range.second;
        if ( range.first > range.second ){
            minRange = range.second;
            maxRange = range.first;
        }
        if ( minRange < maxRange ){
            result = setZoomRange( minRange, maxRange );
        }
    }
    return result;
}


Profiler::~Profiler(){
}
}
}
