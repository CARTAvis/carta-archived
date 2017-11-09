#include <math.h>
#include "DataGrid.h"
#include "AxisMapper.h"
#include "Fonts.h"
#include "Themes.h"
#include "Globals.h"
#include "DummyGridRenderer.h"
#include "CartaLib/AxisInfo.h"
#include "Data/Image/CoordinateSystems.h"
#include "LabelFormats.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataGrid::CLASS_NAME = "DataGrid";
const QString DataGrid::AXES = "axes";
const QString DataGrid::COORD_SYSTEM = "skyCS";
const QString DataGrid::DIRECTION = "direction";
const QString DataGrid::FONT = "font";
const QString DataGrid::LABEL_COLOR = "labels";
const QString DataGrid::LABEL_DECIMAL_PLACES = "decimals";
const QString DataGrid::LABEL_DECIMAL_PLACES_MAX = "decimalsMax";
const QString DataGrid::LABEL_FORMAT = "labelFormats";
const QString DataGrid::LABEL_AXIS = "axis";

const QString DataGrid::FORMAT = "format";
const QString DataGrid::LABEL_SIDE = "side";
const QString DataGrid::SHOW_AXIS = "showAxis";
const QString DataGrid::SHOW_COORDS = "showCoordinateSystem";
const QString DataGrid::SHOW_INTERNAL_LABELS = "showInternalLabels";
const QString DataGrid::SHOW_GRID_LINES = "showGridLines";
const QString DataGrid::SHOW_STATISTICS = "showStatistics";
const QString DataGrid::SHOW_TICKS = "showTicks";
const QString DataGrid::SPACING = "spacing";
const QString DataGrid::GRID = "grid";
const QString DataGrid::THEME = "theme";
const QString DataGrid::TICK = "tick";
const QString DataGrid::TICK_LENGTH = "tickLength";
const QString DataGrid::SUPPORTED_AXES = "supportedAxes";
const int DataGrid::MARGIN_DEFAULT = 10;
const int DataGrid::MARGIN_LABEL = 50;
const int DataGrid::TICK_LENGTH_MAX = 50;
const int DataGrid::PEN_FACTOR = 10;
const int DataGrid::LABEL_DECIMAL_MAX = 10;

using Carta::Lib::AxisInfo;

CoordinateSystems* DataGrid::m_coordSystems = nullptr;
Fonts* DataGrid::m_fonts = nullptr;
Themes* DataGrid::m_themes = nullptr;
LabelFormats* DataGrid::m_formats = nullptr;

class DataGrid::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new DataGrid (path, id);
        }
    };

bool DataGrid::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new DataGrid::Factory());

DataGrid::DataGrid( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_wcsGridRenderer( nullptr){
    m_errorMargin = 0.000001;
    m_borderColor = QColor( 0, 255, 0, 255 );

    _initializeSingletons();
    _initializeDefaultState();
}

void DataGrid::_addUsedPurpose( const QString& key, const QString& targetPurpose,
    QList<QString>& usedPurposes, QString& usedPurposeKey ){
    QString otherPurpose = m_state.getValue<QString>( key );
    usedPurposes.append( otherPurpose );
    if ( otherPurpose == targetPurpose ){
        usedPurposeKey = key;
    }
}

std::vector<AxisInfo::KnownType> DataGrid::_getDisplayAxes() const {
    std::vector<AxisInfo::KnownType> displayTypes( 2 );
    QString xPurposeName = m_state.getValue<QString>( AxisMapper::AXIS_X );
    displayTypes[0] = AxisMapper::getType( xPurposeName );
    QString yPurposeName = m_state.getValue<QString>( AxisMapper::AXIS_Y );
    displayTypes[1] = AxisMapper::getType( yPurposeName );
    return displayTypes;
}

QString DataGrid::_getFormat( const Carta::State::StateInterface& state, const QString& direction ) const {
    QString format = LabelFormats::FORMAT_NONE;
    if ( direction.length() > 0 ){
        QString directionLookup = Carta::State::UtilState::getLookup( LABEL_FORMAT, direction);
        QString directionFormatLookup = Carta::State::UtilState::getLookup( directionLookup, FORMAT );
        format = state.getValue<QString>( directionFormatLookup );
    }
    return format;
}


Carta::Lib::AxisLabelInfo DataGrid::_getAxisLabelInfo( int axisIndex,
        Carta::Lib::AxisInfo::KnownType axisType, const Carta::Lib::KnownSkyCS& cs ) const {
    Carta::Lib::AxisLabelInfo info;
    int precision= m_state.getValue<int>(LABEL_DECIMAL_PLACES );
    info.setPrecision( precision );
    QString labelLocation = _getLabelLocation( m_state, axisIndex );
    if ( labelLocation.length() > 0 ){
        Carta::Lib::AxisLabelInfo::Locations location = m_formats->getAxisLabelLocation( labelLocation );
        info.setLocation( location );

        QString format = _getFormat( m_state, labelLocation );
        if ( m_formats->isDefault( format ) ){
            format = m_formats->getDefaultFormatForAxis( axisType, cs );
        }
        Carta::Lib::AxisLabelInfo::Formats axisFormat = m_formats->getAxisLabelFormat( format );
        info.setFormat( axisFormat );
    }
    return info;
}

QString DataGrid::_getLabelLocation( const Carta::State::StateInterface& state, int axisIndex ) const {
    QString location;
    //Horizontal axis
    if ( axisIndex == 0 ){
        QString northFormat = _getFormat( state, LabelFormats::NORTH );
        if ( m_formats -> isVisible( northFormat ) ){
            location = LabelFormats::NORTH;
        }
        else {
            QString southFormat = _getFormat( state, LabelFormats::SOUTH );
            if ( m_formats ->isVisible( southFormat ) ){
                location = LabelFormats::SOUTH;
            }
        }
    }
    //Vertical axis
    else if ( axisIndex == 1 ){
        QString eastFormat = _getFormat( state, LabelFormats::EAST );
        if ( m_formats -> isVisible( eastFormat ) ){
            location = LabelFormats::EAST;
        }
        else {
            QString westFormat = _getFormat( state, LabelFormats::WEST );
            if ( m_formats -> isVisible( westFormat ) ){
                location = LabelFormats::WEST;
            }
        }
    }
    return location;
}

int DataGrid::_getMargin( const QString& direction ) const {
    QString marginFormat = _getFormat( m_state, direction );
    int margin = MARGIN_DEFAULT;
    if ( m_formats->isVisible( marginFormat ) ){
        margin = MARGIN_LABEL;
    }
    return margin;
}

Carta::Lib::KnownSkyCS DataGrid::_getSkyCS() const {
    // coordinate system
    QString coordSystem = m_state.getValue<QString>( COORD_SYSTEM );
    Carta::Lib::KnownSkyCS index = m_coordSystems->getIndex( coordSystem);
    return index;
}


QPen DataGrid::_getPen( const QString& key, const Carta::State::StateInterface& state  ){
    QString redLookup = Carta::State::UtilState::getLookup( key, Util::RED );
    int redAmount = state.getValue<int>( redLookup );
    QString greenLookup = Carta::State::UtilState::getLookup( key, Util::GREEN );
    int greenAmount = state.getValue<int>( greenLookup );
    QString blueLookup = Carta::State::UtilState::getLookup( key, Util::BLUE );
    int blueAmount = state.getValue<int>( blueLookup );
    QString alphaLookup = Carta::State::UtilState::getLookup( key, Util::ALPHA );
    int alphaAmount = state.getValue<int>( alphaLookup );
    QString widthLookup = Carta::State::UtilState::getLookup( key, Util::WIDTH );
    int widthAmount = state.getValue<int>( widthLookup );
    QPen pen( QColor(redAmount, greenAmount, blueAmount, alphaAmount));
    pen.setWidth( widthAmount);
    return pen;
}

std::shared_ptr<Carta::Lib::IWcsGridRenderService> DataGrid::_getRenderer(){
    return m_wcsGridRenderer;
}

Carta::State::StateInterface DataGrid::_getState(){
    return m_state;
}


void DataGrid::_initializeGridRenderer(){
    auto res = Globals::instance()-> pluginManager()
                   -> prepare < Carta::Lib::Hooks::GetWcsGridRendererHook > ().first();
    if ( res.isNull() || ! res.val() ) {
        qWarning( "wcsgrid: Could not find any WCS grid renderers" );
        m_wcsGridRenderer = nullptr;
    }
    else {
        m_wcsGridRenderer = res.val();
    }
    if ( ! m_wcsGridRenderer ) {
        qWarning( "wcsgrid: Creating dummy grid renderer" );
        m_wcsGridRenderer.reset( new Carta::Core::DummyGridRenderer() );
    }

    _resetGridRenderer();
    m_wcsGridRenderer-> setEmptyGrid( false );
}

void DataGrid::_initializeDefaultPen( const QString& key, int red, int green, int blue,
        int alpha, int width ){
    m_state.insertObject( key );
    QString redLookup = Carta::State::UtilState::getLookup( key, Util::RED );
    m_state.insertValue<int>( redLookup, red );
    QString blueLookup = Carta::State::UtilState::getLookup( key, Util::BLUE );
    m_state.insertValue<int>( blueLookup, blue );
    QString greenLookup = Carta::State::UtilState::getLookup( key, Util::GREEN );
    m_state.insertValue<int>( greenLookup, green );
    QString alphaLookup = Carta::State::UtilState::getLookup( key, Util::ALPHA );
    m_state.insertValue<int>( alphaLookup, alpha );
    if ( width >= 0 ){
        QString widthLookup = Carta::State::UtilState::getLookup( key, Util::WIDTH );
        m_state.insertValue<int>( widthLookup, width );
    }
}

void DataGrid::_initializeLabelFormat( const QString& side, const QString& format,
        AxisInfo::KnownType axis ){
    QString lookup = Carta::State::UtilState::getLookup( LABEL_FORMAT, side );
    m_state.insertObject( lookup );
    QString formatLookup = Carta::State::UtilState::getLookup( lookup, FORMAT );
    m_state.insertValue<QString>( formatLookup, format );
    QString axisLookup = Carta::State::UtilState::getLookup( lookup, LABEL_AXIS );
    int axisVal = static_cast<int>(axis);
    m_state.insertValue<int>(axisLookup, axisVal );
}

void DataGrid::_initializeDefaultState(){

    QString defaultSystem = m_coordSystems->getDefault();
    m_state.insertValue<QString>( COORD_SYSTEM, defaultSystem);

    //Thickness seems to go from 0 to 20 - double
    //Transparency seems to go from 0 to 1
    //Spacing goes from 0.250 to 3

    m_state.insertValue<bool>( SHOW_AXIS, false );
    m_state.insertValue<bool>( SHOW_GRID_LINES, false );
    m_state.insertValue<bool>( SHOW_TICKS, true );
    m_state.insertValue<bool>( SHOW_INTERNAL_LABELS, false );
    m_state.insertValue<bool>( SHOW_COORDS, true );

    m_state.insertValue<bool>( SHOW_STATISTICS, true );

    //Pens
    _initializeDefaultPen( GRID, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( AXES, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( TICK, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    m_state.insertValue<double>( SPACING, .33 );
    m_state.insertValue<int>(TICK_LENGTH, 5 );

    //Label Color
    _initializeDefaultPen( LABEL_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );

    //Fonts
    m_state.insertObject( FONT );
    QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
    QString defaultFont = m_fonts->getDefaultFamily();
    m_state.insertValue<QString>( familyLookup, m_fonts->getDefaultFamily());
    QString sizeLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_SIZE );
    m_state.insertValue<int>( sizeLookup, m_fonts->getDefaultSize());

    m_state.insertValue<QString>(THEME,m_themes->getDefaultTheme());

    m_state.insertValue<int>(LABEL_DECIMAL_PLACES, 3 );
    m_state.insertValue<int>(LABEL_DECIMAL_PLACES_MAX, LABEL_DECIMAL_MAX );
    m_state.insertObject( LABEL_FORMAT );
    QString formatEast = m_formats->getDefaultFormat( LabelFormats::EAST );
    _initializeLabelFormat( LabelFormats::EAST, formatEast, AxisInfo::KnownType::DIRECTION_LAT );
    QString formatWest = m_formats->getDefaultFormat( LabelFormats::WEST );
    _initializeLabelFormat( LabelFormats::WEST, formatWest, AxisInfo::KnownType::DIRECTION_LAT );
    QString formatNorth = m_formats->getDefaultFormat( LabelFormats::NORTH );
    _initializeLabelFormat( LabelFormats::NORTH, formatNorth, AxisInfo::KnownType::DIRECTION_LON );
    QString formatSouth = m_formats->getDefaultFormat( LabelFormats::SOUTH );
    _initializeLabelFormat( LabelFormats::SOUTH, formatSouth, AxisInfo::KnownType::DIRECTION_LON );

    m_state.insertArray( SUPPORTED_AXES, 0);
    const Carta::Lib::KnownSkyCS& cs = m_coordSystems->getDefaultType();
    QString xName = AxisMapper::getDefaultPurpose( AxisMapper::AXIS_X, cs );
    m_state.insertValue<QString>( AxisMapper::AXIS_X, xName );
    QString yName = AxisMapper::getDefaultPurpose( AxisMapper::AXIS_Y, cs );
    m_state.insertValue<QString>( AxisMapper::AXIS_Y, yName );

    m_state.flushState();
}



void DataGrid::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_coordSystems == nullptr ){
        m_coordSystems = Util::findSingletonObject<CoordinateSystems>();
    }

    //Available fonts
    if ( m_fonts == nullptr ){
        m_fonts = Util::findSingletonObject<Fonts>();
    }

    //Available themes
    if ( m_themes == nullptr ){
        m_themes = Util::findSingletonObject<Themes>();
    }

    //Label formats
    if ( m_formats == nullptr ){
        m_formats = Util::findSingletonObject<LabelFormats>();
    }
}

bool DataGrid::_isGridVisible() const {
    bool gridLines =  m_state.getValue<bool>(SHOW_GRID_LINES );
    bool axes = m_state.getValue<bool>( SHOW_AXIS );
    bool gridVisible = gridLines || axes;
    return gridVisible;
}


// Use to set the displayed coordinate system in UI
void DataGrid::_resetCoordinateSystem( const QString& coordSystem ){
    if ( m_state.getValue<QString>( COORD_SYSTEM) != coordSystem ){
        m_state.setValue<QString>( COORD_SYSTEM, coordSystem );
    }
}

void DataGrid::_resetGridRenderer(){
    if ( m_wcsGridRenderer.get() != nullptr){

        bool showCoords = m_state.getValue<bool>( SHOW_COORDS );
        m_wcsGridRenderer-> setEmptyGrid( !showCoords );

        //Internal Labels
        bool internalLabels = m_state.getValue<bool>( SHOW_INTERNAL_LABELS );
        m_wcsGridRenderer->setInternalLabels( internalLabels );

        //Grid Spacing seems to go from 0.25 to 1.75
        double spacing = m_state.getValue<double>(SPACING);
        double densityModifier = 0.25 + spacing * 1.5;
        m_wcsGridRenderer-> setGridDensityModifier( densityModifier );

        //Grid
        bool showGrid = m_state.getValue<bool>(SHOW_GRID_LINES);
        m_wcsGridRenderer->setGridLinesVisible( showGrid );

        QPen gridPen = _getPen( GRID, m_state );
        QColor penColor = gridPen.color();
        gridPen.setColor( penColor );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines1,
                                            gridPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines2,
                                           gridPen);

        //Border
        QPen borderPen( m_borderColor );
        m_wcsGridRenderer->setPen( Carta::Lib::IWcsGridRenderService::Element::MarginDim, borderPen );

        //Ticks
        bool showTick = m_state.getValue<bool>( SHOW_TICKS );
        m_wcsGridRenderer->setTicksVisible( showTick );
        QPen tickPen = _getPen( TICK, m_state );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines1,
                                    tickPen );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines2,
                                    tickPen);
        int tickLength = m_state.getValue<int>(TICK_LENGTH);
        double gridTickLength = tickLength / 1000.0;
        m_wcsGridRenderer->setTickLength( gridTickLength );

        //AXES
        bool showAxes = m_state.getValue<bool>( SHOW_AXIS );
        m_wcsGridRenderer->setAxesVisible( showAxes );

        QPen axesPen = _getPen( AXES, m_state );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines1,
                                    axesPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines2,
                                    axesPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::BorderLines,
                                    axesPen );

        //Label
        QPen labelPen = _getPen( LABEL_COLOR, m_state );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText1,
                                    labelPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText2,
                                    labelPen );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText1,
                                    labelPen );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText2,
                                    labelPen );

        QPen shadowPen( QColor( 0,0,0,64));
        shadowPen.setWidth(3);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::Shadow,
                                        shadowPen);

        QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
        QString fontFamily = m_state.getValue<QString>( familyLookup );
        int fontIndex = m_fonts->getIndex( fontFamily );
        if ( fontIndex >= 0 ){
            QString sizeLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_SIZE );
            int fontSize = m_state.getValue<int>( sizeLookup );
            m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::NumText1,
                                     fontIndex, fontSize);
            m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::NumText2,
                                     fontIndex, fontSize);
            m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::LabelText1,
                                     fontIndex, fontSize);
            m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::LabelText2,
                                     fontIndex, fontSize);
        }

        // coordinate system
        QString coordSystem = m_state.getValue<QString>( COORD_SYSTEM );
        Carta::Lib::KnownSkyCS index = m_coordSystems->getIndex( coordSystem);
        m_wcsGridRenderer-> setSkyCS( index );

    }
}



void DataGrid::_resetState( const Carta::State::StateInterface& otherState ){
    if ( otherState.getValue<QString>( Carta::State::StateInterface::OBJECT_TYPE).length() > 0 ){
        m_state = otherState;
        _resetGridRenderer();
    }
}

QString DataGrid::_setAxis( const QString& axisId, const QString& purpose, bool* axisChanged ){
    QString result;
    *axisChanged = false;
    QString actualAxis = AxisMapper::getDisplayName( axisId );
    if ( !actualAxis.isEmpty() ){
        if ( !purpose.isEmpty() ){
            QString oldPurpose = m_state.getValue<QString>( axisId );
            if ( oldPurpose != purpose ){

                //Get a running total of display axes that are being used; at the same time store
                //the key of the axis containing the duplicate name, if there is one.
                QList<QString> usedPurposes;
                QString duplicateAxisPurposeKey;
                QStringList axisNames = AxisMapper::getDisplayNames();
                int displayAxisCount = axisNames.size();
                for ( int i = 0; i < displayAxisCount; i++ ){
                    if ( actualAxis != axisNames[i] ){
                        _addUsedPurpose( axisNames[i], purpose, usedPurposes, duplicateAxisPurposeKey );
                    }
                }

                //There is an axis that is already displaying the one that we want to display.  Thus,
                //we need to find a new purpose for the duplicate.
                if ( !duplicateAxisPurposeKey.isEmpty() ){
                    int purposeCount = m_state.getArraySize( SUPPORTED_AXES );
                    for ( int i = 0; i < purposeCount; i++ ){
                        QString lookup = Carta::State::UtilState::getLookup( SUPPORTED_AXES, i );
                        QString axisPurpose = m_state.getValue<QString>( lookup );
                        if ( !usedPurposes.contains( axisPurpose ) && axisPurpose.length() > 0 ){
                            m_state.setValue<QString>(duplicateAxisPurposeKey, axisPurpose );
                            break;
                        }
                    }
                }
                *axisChanged = true;
                m_state.setValue<QString>( actualAxis, purpose );

                m_state.flushState();

                // update Label format
                std::vector<AxisInfo::KnownType> AxisTypeArray = _getDisplayAxes();
                m_formats->setAxisformat(&AxisTypeArray[0]);
            }
        }
        else {
            result = "Unrecognized axis type: "+purpose;
        }
    }
    else {
        result = "Unrecognized axis: "+axisId;
    }
    return result;
}


QStringList DataGrid::_setAxesColor( int redAmount, int greenAmount, int blueAmount,
        bool* axesColorChanged ){
    QStringList result = _setColor( AXES, redAmount, greenAmount, blueAmount, axesColorChanged );
    return result;
}

QString DataGrid::_setAxesTransparency( int transparency, bool* transparencyChanged ){
    *transparencyChanged = false;
    QString result;
    if ( transparency < 0 || transparency > Util::MAX_COLOR ){
        result = "Axes transparency must be in [0,"+QString::number( Util::MAX_COLOR) +
                "]: "+ QString::number( transparency );
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( AXES, Util::ALPHA );
        double oldTransparency = m_state.getValue<double>(lookup);
        if ( transparency != oldTransparency ){
            m_state.setValue<int>( lookup, transparency);
            *transparencyChanged = true;
        }
    }
    return result;
}

QString DataGrid::_setAxesThickness( int thickness, bool* thicknessChanged ){
    *thicknessChanged = false;
    QString result;
    if ( thickness < 0 || thickness > PEN_FACTOR ){
        result = "Axes thickness must be in [0,"+QString::number(PEN_FACTOR)+
                "1]: "+QString::number(thickness);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( AXES, Util::WIDTH );
        int oldThickness = m_state.getValue<int>(lookup);
        if ( oldThickness != thickness ){
            m_state.setValue<int>( lookup, thickness);
            *thicknessChanged = true;
        }
    }
    return result;
}

bool DataGrid::_setAxisTypes( std::vector<AxisInfo::KnownType> supportedAxes){
    int axisCount = supportedAxes.size();
    bool axisTypesChanged = false;
    int oldCount = m_state.getArraySize( SUPPORTED_AXES );
    m_state.resizeArray( SUPPORTED_AXES, axisCount, Carta::State::StateInterface::PreserveAll );
    QString coordStr = m_state.getValue<QString>( COORD_SYSTEM );
    const Carta::Lib::KnownSkyCS& cs = m_coordSystems->getIndex( coordStr );
    for ( int i = 0; i < axisCount; i++ ){
        QString name = AxisMapper::getPurpose( supportedAxes[i] );
        QString lookup = Carta::State::UtilState::getLookup( SUPPORTED_AXES, i );
        QString oldName;
        if ( i < oldCount ){
            oldName = m_state.getValue<QString>( lookup );
        }
        if ( name != oldName ){
            axisTypesChanged = true;
            m_state.setValue<QString>( lookup, name );
        }
    }
    if ( axisTypesChanged ){
        m_state.flushState();
    }
    return axisTypesChanged;
}

bool DataGrid::_setAxisInfos( std::vector<AxisInfo> supportedAxes){
    int axisCount = supportedAxes.size();
    bool axisTypesChanged = false;
    int oldCount = m_state.getArraySize( SUPPORTED_AXES );
    m_state.resizeArray( SUPPORTED_AXES, axisCount, Carta::State::StateInterface::PreserveAll );

    int axisIndex = 0;
    for( int i=0; i<axisCount; i++ ){
        QString name = supportedAxes[i].longLabel().plain();
        QString lookup = Carta::State::UtilState::getLookup( SUPPORTED_AXES, axisIndex );
        QString oldName;
        if ( i < oldCount ){
            oldName = m_state.getValue<QString>( lookup );
        }
        if ( name != oldName ){
            //_setAxis( AxisMapper::AXIS_X, supportedAxes[0].longLabel().plain(), &axisTypesChanged);
            //_setAxis( AxisMapper::AXIS_Y, supportedAxes[1].longLabel().plain(), &axisTypesChanged);
            //if(i==0){
            //    _setAxis( AxisMapper::AXIS_X, name, &axisTypesChanged);
            //}
            //else if(i==1){
            //    _setAxis( AxisMapper::AXIS_Y, name, &axisTypesChanged);
            //}
            axisTypesChanged = true;
            if ( AxisMapper::getType(name)==Carta::Lib::AxisInfo::KnownType::STOKES ){
                m_state.resizeArray( SUPPORTED_AXES, axisCount-1, Carta::State::StateInterface::PreserveAll);
                continue;
            }
            m_state.setValue<QString>( lookup, name );
        }
        axisIndex++;
    }

    if ( axisTypesChanged ){
        m_state.flushState();
    }
    return axisTypesChanged;
}

void DataGrid::_setBorderColor( QColor borderColor ){
    if ( m_borderColor != borderColor ){
        m_borderColor = borderColor;
        _resetGridRenderer();
    }
}

QStringList DataGrid::_setColor( const QString& key, int redAmount, int greenAmount, int blueAmount,
        bool* colorChanged ){
    QStringList result;
    if ( redAmount < 0 || redAmount > Util::MAX_COLOR ){
        result.append("Red must be in [0,"+QString::number( Util::MAX_COLOR) +
                "]: "+QString::number( redAmount ));
    }
    if ( greenAmount < 0 || greenAmount > Util::MAX_COLOR ){
        result.append("Green must be in [0,"+QString::number(Util::MAX_COLOR)+"]: "+
                QString::number( greenAmount ));
    }
    if ( blueAmount < 0 || blueAmount > Util::MAX_COLOR ){
        result.append("Blue amount must be in [0,"+ QString::number(Util::MAX_COLOR)+
                "]: "+QString::number( blueAmount ));
    }
    if ( result.size() == 0 ){
        *colorChanged = false;
        QString redLookup = Carta::State::UtilState::getLookup( key, Util::RED );
        if ( m_state.getValue<int>(redLookup) != redAmount ){
            m_state.setValue<int>( redLookup, redAmount );
            *colorChanged = true;
        }
        QString blueLookup = Carta::State::UtilState::getLookup( key, Util::BLUE );
        if ( m_state.getValue<int>(blueLookup) != blueAmount ){
            m_state.setValue<int>( blueLookup, blueAmount );
            *colorChanged = true;
        }
        QString greenLookup = Carta::State::UtilState::getLookup( key, Util::GREEN );
        if ( m_state.getValue<int>( greenLookup ) != greenAmount ){
            m_state.setValue<int>( greenLookup, greenAmount );
            *colorChanged = true;
        }
        if ( *colorChanged ){
            m_state.flushState();
        }
    }
    return result;
}

QString DataGrid::_setCoordinateSystem( const QString& coordSystem, bool* coordChanged ){
    QString result;
    *coordChanged = false;
    QString recognizedSystem = m_coordSystems->getCoordinateSystem( coordSystem );
    if ( !recognizedSystem.isEmpty() ){
        if ( m_state.getValue<QString>( COORD_SYSTEM) != recognizedSystem ){
            m_state.setValue<QString>( COORD_SYSTEM, recognizedSystem );
            *coordChanged = true;
        }
    }
    else {
        result= "The coordinate system "+coordSystem+" is not supported.";
    }
    return result;
}

QString DataGrid::_setFontFamily( const QString& fontFamily, bool* familyChanged ){
    QString result;
    *familyChanged = false;
    QString recognizedFont = m_fonts->getFontFamily( fontFamily );
    if ( !recognizedFont.isEmpty() ){
        QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
        if ( m_state.getValue<QString>( familyLookup ) != recognizedFont ){
            m_state.setValue<QString>( familyLookup, recognizedFont );
            *familyChanged = true;
        }
    }
    else {
        result= "The font family "+fontFamily+" is not supported.";
    }
    return result;
}

QString DataGrid::_setFontSize( int fontSize, bool* sizeChanged ){
    *sizeChanged = false;
    QString result;
    bool sizeSupported = m_fonts->isFontSize( fontSize );
    if ( !sizeSupported ){
        result = "The font size is not supported: "+QString::number(fontSize);
    }
    else {
        QString sizeLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_SIZE );
        double oldSize = m_state.getValue<int>( sizeLookup );
        if ( oldSize != fontSize){
            m_state.setValue<int>( sizeLookup, fontSize);
            *sizeChanged = true;
        }
    }
    return result;
}


QStringList DataGrid::_setGridColor( int redAmount, int greenAmount, int blueAmount,
        bool* gridColorChanged ){
    QStringList result = _setColor( GRID, redAmount, greenAmount, blueAmount, gridColorChanged );
    return result;
}

QString DataGrid::_setGridSpacing( double spacing, bool* spacingChanged ){
    *spacingChanged = false;
    QString result;
    if ( spacing < 0 || spacing > 1 ){
        result = "Grid spacing must be in [0,1]: "+QString::number(spacing);
    }
    else {
        double oldSpacing = m_state.getValue<double>(SPACING);
        if ( fabs( oldSpacing - spacing ) > m_errorMargin ){
            m_state.setValue<double>( SPACING, spacing);
            *spacingChanged = true;
        }
    }
    return result;
}

QString DataGrid::_setGridThickness( int thickness, bool* coordChanged ){
    *coordChanged = false;
    QString result;
    if ( thickness < 0 || thickness > PEN_FACTOR ){
        result = "Grid thickness must be in [0,"+QString::number(PEN_FACTOR)+"]: "+
                QString::number(thickness);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( GRID, Util::WIDTH );
        int oldThickness = m_state.getValue<int>(lookup);
        if ( oldThickness != thickness ){
            m_state.setValue<int>( lookup, thickness);
            *coordChanged = true;
        }
    }
    return result;
}

QStringList DataGrid::_setLabelColor( int redAmount, int greenAmount, int blueAmount,
        bool* labelColorChanged ){
    QStringList result = _setColor( LABEL_COLOR, redAmount, greenAmount, blueAmount, labelColorChanged );
    return result;
}

QString DataGrid::_setLabelDecimalPlaces( int decimalPlaces, bool* decimalsChanged ){
    QString result;
    if ( decimalPlaces < 0 ){
        result = "Axis label decimal places must be nonnegative.";
    }
    else if ( decimalPlaces > LABEL_DECIMAL_MAX ){
        result = "Axis label decimal places can not exceed "+QString::number( LABEL_DECIMAL_MAX )+".";
    }
    else {
        int decimalPlacesOld = m_state.getValue<int>( LABEL_DECIMAL_PLACES );
        if ( decimalPlacesOld != decimalPlaces ){
            *decimalsChanged = true;
            m_state.setValue<int>( LABEL_DECIMAL_PLACES, decimalPlaces );
        }
    }
    return result;
}

QString DataGrid::_setLabelFormat( const QString& side, const QString& format, bool* labelFormatChanged ){
    QString result;
    //TODO:  Need to put in a check that the format makes sense the type of axis
    //displayed on the indicated side.
    *labelFormatChanged = false;
    QString actualSide = m_formats->getDirection( side );
    if ( actualSide.isEmpty() ){
        result = "Unrecognized label side: "+side;
    }
    else {
        QString actualFormat = m_formats->getFormat( format );
        if ( actualFormat.isEmpty() ){
            result = "Unrecognized label format: "+format;
        }
        else {
            QString oldFormat = _getFormat( m_state, actualSide);
            if ( oldFormat != actualFormat ){
                bool visible = m_formats->isVisible( actualFormat );
                QString directionLookup = Carta::State::UtilState::getLookup( LABEL_FORMAT, side);
                QString directionFormatLookup = Carta::State::UtilState::getLookup( directionLookup, FORMAT );
                m_state.setValue<QString>( directionFormatLookup, actualFormat );
                if ( visible ){
                    //Make sure the format on the opposite side makes the labels
                    //invisible.
                    QString oppositeSide = m_formats->getOppositeSide( side );
                    QString dLookup = Carta::State::UtilState::getLookup( LABEL_FORMAT, oppositeSide );
                    QString dFormatLookup = Carta::State::UtilState::getLookup( dLookup, FORMAT );
                    m_state.setValue<QString>( dFormatLookup, LabelFormats::FORMAT_NONE );
                }

                *labelFormatChanged = true;
            }
        }
    }
    return result;
}

QString DataGrid::_setGridTransparency( int transparency, bool* transparencyChanged ){
    *transparencyChanged = false;
    QString result;
    if ( transparency < 0 || transparency > Util::MAX_COLOR ){
        result = "Grid transparency must be in [0,"+QString::number(Util::MAX_COLOR)+
                "]: "+ QString::number( transparency );
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( GRID, Util::ALPHA );
        double oldTransparency = m_state.getValue<double>(lookup);
        if ( transparency != oldTransparency ){
            m_state.setValue<int>( lookup, transparency);
            *transparencyChanged = true;
        }
    }
    return result;
}


QString DataGrid::_setShowAxis( bool showAxis, bool* coordChanged ){
    QString result;
    *coordChanged = false;
    if ( m_state.getValue<bool>(SHOW_AXIS) != showAxis ){
        m_state.setValue<bool>(SHOW_AXIS, showAxis );
        *coordChanged = true;
    }
    return result;
}

QString DataGrid::_setShowCoordinateSystem( bool showCoordinateSystem, bool* coordChanged ){
    QString result;
    *coordChanged = false;
    if ( m_state.getValue<bool>(SHOW_COORDS) != showCoordinateSystem ){
        m_state.setValue<bool>( SHOW_COORDS, showCoordinateSystem );
        *coordChanged = true;
    }
    return result;
}

QString DataGrid::_setShowGridLines( bool showLines, bool* coordChanged ){
    QString result;
    *coordChanged = false;
    if ( m_state.getValue<bool>(SHOW_GRID_LINES) != showLines ){
        m_state.setValue<bool>(SHOW_GRID_LINES, showLines );
        *coordChanged = true;
    }
    return result;
}


QString DataGrid::_setShowInternalLabels( bool showInternalLabels, bool* coordChanged ){
    *coordChanged = false;
    QString result;
    bool oldValue = m_state.getValue<bool>( SHOW_INTERNAL_LABELS );
    if ( oldValue != showInternalLabels ){
        *coordChanged = true;
        m_state.setValue<bool>( SHOW_INTERNAL_LABELS, showInternalLabels );
    }
    return result;
}

QString DataGrid::_setShowStatistics( bool showStatistics, bool* statisticsChanged ){
    *statisticsChanged = false;
    QString result;
    bool oldValue = m_state.getValue<bool>( SHOW_STATISTICS );
    if ( oldValue != showStatistics ){
        *statisticsChanged = true;
        m_state.setValue<bool>( SHOW_STATISTICS, showStatistics );
    }
    return result;
}

QString DataGrid::_setShowTicks( bool showTicks, bool* ticksChanged ){
    *ticksChanged = false;
    QString result;
    bool oldValue = m_state.getValue<bool>( SHOW_TICKS );
    if ( oldValue != showTicks ){
        *ticksChanged = true;
        m_state.setValue<bool>( SHOW_TICKS, showTicks );
    }
    return result;
}



QStringList DataGrid::_setTickColor( int redAmount, int greenAmount, int blueAmount, bool* colorChanged ){
    QStringList result = _setColor( TICK, redAmount, greenAmount, blueAmount, colorChanged );
    return result;
}

QString DataGrid::_setTickLength( int tickLength, bool* lengthChanged ){
    *lengthChanged = false;
    QString result;
    if ( tickLength < 0 || tickLength > TICK_LENGTH_MAX ){
        result = "Tick length must be in [0,"+QString::number(TICK_LENGTH_MAX)+
                "]: "+QString::number(tickLength);
    }
    else {
        int oldLength = m_state.getValue<int>(TICK_LENGTH);
        if ( oldLength != tickLength ){
            m_state.setValue<int>( TICK_LENGTH, tickLength);
            *lengthChanged = true;
        }
    }
    return result;
}

QString DataGrid::_setTickThickness( int tickThickness, bool* thicknessChanged ){
    *thicknessChanged = false;
    QString result;
    if ( tickThickness < 0 || tickThickness > PEN_FACTOR ){
        result = "Tick thickness must be in [0,"+QString::number(PEN_FACTOR)+
                "]: "+QString::number(tickThickness);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( TICK, Util::WIDTH );
        int oldThickness = m_state.getValue<int>(lookup);
        if ( oldThickness != tickThickness ){
            m_state.setValue<int>( lookup, tickThickness);
            *thicknessChanged = true;
        }
    }
    return result;
}

QString DataGrid::_setTickTransparency( int transparency, bool* transparencyChanged ){
    *transparencyChanged = false;
    QString result;
    if ( transparency < 0 || transparency > Util::MAX_COLOR ){
        result = "Tick transparency must be in [0,255]: "+ QString::number( transparency );
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( TICK, Util::ALPHA );
        double oldTransparency = m_state.getValue<double>(lookup);
        if ( transparency != oldTransparency ){
            m_state.setValue<int>( lookup, transparency);
            *transparencyChanged = true;
        }
    }
    return result;
}

QString DataGrid::_setTheme( const QString& theme, bool* themeChanged ){
    *themeChanged = false;
    QString result;
    QString recognizedTheme = m_themes->getTheme( theme );
    if ( !recognizedTheme.isEmpty() ){
        QString oldTheme = m_state.getValue<QString>( THEME );
        if ( oldTheme != recognizedTheme ){
            m_state.setValue<QString>( THEME, recognizedTheme );
            *themeChanged = true;
        }
    }
    else {
        result = "The theme "+theme+" was not recognized";
    }
    return result;
}

DataGrid::~DataGrid(){

}
}
}
