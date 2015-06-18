#include "DataGrid.h"
#include "CoordinateSystems.h"
#include "Fonts.h"
#include "Globals.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataGrid::CLASS_NAME = "DataGrid";
const QString DataGrid::ALPHA = "alpha";
const QString DataGrid::AXES = "axes";
const QString DataGrid::BLUE = "blue";
const QString DataGrid::COORD_SYSTEM = "skyCS";
const QString DataGrid::DIRECTION = "direction";
const QString DataGrid::FONT = "font";
const QString DataGrid::GREEN = "green";
const QString DataGrid::LABEL_COLOR = "labels";
const QString DataGrid::PEN_WIDTH = "width";
const QString DataGrid::RED = "red";
const QString DataGrid::SHOW_AXIS = "showAxis";
const QString DataGrid::SHOW_INTERNAL_LABELS = "showInternalLabels";
const QString DataGrid::SHOW_GRID_LINES = "showGridLines";
const QString DataGrid::SPACING = "spacing";
const QString DataGrid::GRID = "grid";
const QString DataGrid::TRANSPARENCY = "transparency";
const int DataGrid::PEN_FACTOR = 10;

CoordinateSystems* DataGrid::m_coordSystems = nullptr;
Fonts* DataGrid::m_fonts = nullptr;

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

    _initializeSingletons();
    _initializeDefaultState();
}

QPen DataGrid::_getPen( const QString& key, const Carta::State::StateInterface& state  ){
    QString redLookup = Carta::State::UtilState::getLookup( key, RED );
    int redAmount = state.getValue<int>( redLookup );
    QString greenLookup = Carta::State::UtilState::getLookup( key, GREEN );
    int greenAmount = state.getValue<int>( greenLookup );
    QString blueLookup = Carta::State::UtilState::getLookup( key, BLUE );
    int blueAmount = state.getValue<int>( blueLookup );
    QString alphaLookup = Carta::State::UtilState::getLookup( key, ALPHA );
    int alphaAmount = state.getValue<int>( alphaLookup );
    QString widthLookup = Carta::State::UtilState::getLookup( key, PEN_WIDTH );
    double widthAmount = state.getValue<double>( widthLookup );
    QPen pen( QColor(redAmount, greenAmount, blueAmount, alphaAmount));
    pen.setWidthF( widthAmount * PEN_FACTOR);
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
    }
    else {
        m_wcsGridRenderer = res.val();
        _resetGridRenderer();
        m_wcsGridRenderer-> setEmptyGrid( false );
    }
}

void DataGrid::_initializeDefaultPen( const QString& key, int red, int green, int blue,
        int alpha, double width ){
    m_state.insertObject( key );
    QString redLookup = Carta::State::UtilState::getLookup( key, RED );
    m_state.insertValue<int>( redLookup, red );
    QString blueLookup = Carta::State::UtilState::getLookup( key, BLUE );
    m_state.insertValue<int>( blueLookup, blue );
    QString greenLookup = Carta::State::UtilState::getLookup( key, GREEN );
    m_state.insertValue<int>( greenLookup, green );
    QString alphaLookup = Carta::State::UtilState::getLookup( key, ALPHA );
    m_state.insertValue<int>( alphaLookup, alpha );
    if ( width >= 0 ){
        QString widthLookup = Carta::State::UtilState::getLookup( key, PEN_WIDTH );
        m_state.insertValue<double>( widthLookup, width );
    }
}



void DataGrid::_initializeDefaultState(){

    QString defaultSystem = m_coordSystems->getDefault();
    m_state.insertValue<QString>( COORD_SYSTEM, defaultSystem);

    //Thickness seems to go from 0 to 20 - double
    //Transparency seems to go from 0 to 1
    //Spacing goes from 0.250 to 3

    //Plan is to store all the state as ints from 0 to 1000 and
    //then translate on the c++ to the actual values.
    m_state.insertValue<bool>( SHOW_AXIS, true );
    m_state.insertValue<bool>( SHOW_GRID_LINES, true );
    m_state.insertValue<bool>( SHOW_INTERNAL_LABELS, false );

    //Pens
    _initializeDefaultPen( GRID, 255, 0, 0, 255, .2 );
    _initializeDefaultPen( AXES, 0,255,0,255, .2 );
    m_state.insertValue<double>( TRANSPARENCY, .33 );
    m_state.insertValue<double>( SPACING, .33 );

    //Label Color
    _initializeDefaultPen( LABEL_COLOR, 0, 0,255,255, 1 );

    //Fonts
    m_state.insertObject( FONT );
    QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
    QString defaultFont = m_fonts->getDefaultFamily();
    m_state.insertValue<QString>( familyLookup, m_fonts->getDefaultFamily());
    QString sizeLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_SIZE );
    m_state.insertValue<int>( sizeLookup, m_fonts->getDefaultSize());

    m_state.flushState();
}



void DataGrid::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_coordSystems == nullptr ){
        CartaObject* obj = Util::findSingletonObject( CoordinateSystems::CLASS_NAME );
        m_coordSystems = dynamic_cast<CoordinateSystems*>( obj );
    }
    //Available fonts
    if ( m_fonts == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Fonts::CLASS_NAME );
        m_fonts = dynamic_cast<Fonts*>( obj );

    }
}


void DataGrid::_resetGridRenderer(){
    if ( m_wcsGridRenderer.get() != nullptr){

        //Internal Labels
        bool internalLabels = m_state.getValue<bool>( SHOW_INTERNAL_LABELS );
        m_wcsGridRenderer->setInternalLabels( internalLabels );

        //Grid Spacing seems to go from 0.25 to 3
        double spacing = m_state.getValue<double>(SPACING);
        double densityModifier = 0.25 + spacing * 2.75;
        m_wcsGridRenderer-> setGridDensityModifier( densityModifier );

        //Grid
        QPen gridPen = _getPen( GRID, m_state );
        bool showGrid = m_state.getValue<bool>(SHOW_GRID_LINES);
        QColor penColor = gridPen.color();
        if ( !showGrid ){
            penColor.setAlpha( 0 );
        }
        else {
            double transparency = m_state.getValue<double>( TRANSPARENCY );
            penColor.setAlphaF( transparency );
        }
        gridPen.setColor( penColor );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines1,
                                            gridPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines2,
                                           gridPen);

        QPen tickLine1Pen( QColor( "white"));
        tickLine1Pen.setWidth( 1 );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines1,
                                    tickLine1Pen );
        QPen tickLine2Pen( QColor( "yellow"));
        tickLine2Pen.setWidth( 1 );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines2,
                                    tickLine2Pen);

        //AXES
        QPen axesPen = _getPen( AXES, m_state );
        bool showAxes = m_state.getValue<bool>( SHOW_AXIS );
        if ( !showAxes ){
            QColor penColor = axesPen.color();
            penColor.setAlpha( 0 );
            axesPen.setColor( penColor );
        }
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines1,
                                    axesPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines2,
                                    axesPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::BorderLines,
                                    axesPen );

        //Label
        QPen labelPen = _getPen( LABEL_COLOR, m_state );
        QColor labelColor = labelPen.color();
        qDebug() << "Label color red="<<labelColor.red()<<" green="<<labelColor.green()<<" blue="<<labelColor.blue();
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText1,
                                    labelPen);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText2,
                                    labelPen );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText1,
                                    labelPen );
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText2,
                                    labelPen );

        QPen marginDimPen( QColor( 0,0,0,64) );
        marginDimPen.setWidth(1);
        m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::MarginDim,
                                    marginDimPen);

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

bool DataGrid::_resetState( const Carta::State::StateInterface& otherState ){
    bool stateChanged = false;

    bool internalChanged = false;
    bool newInternalLabels = otherState.getValue<bool>( SHOW_INTERNAL_LABELS );
    _setShowInternalLabels( newInternalLabels, &internalChanged );

    bool axesChanged = false;
    bool newShowAxes = otherState.getValue<bool>( SHOW_AXIS );
    _setShowAxis( newShowAxes, &axesChanged );

    bool gridLinesChanged = false;
    bool newGridLines = otherState.getValue<bool>( SHOW_GRID_LINES );
    _setShowGridLines( newGridLines, &gridLinesChanged );

    //Coordinate system
    bool coordChanged = false;
    QString newCoords = otherState.getValue<QString>(COORD_SYSTEM);
    _setCoordinateSystem( newCoords, &coordChanged);

    bool gridThicknessChanged = false;
    QString thicknessLookup = Carta::State::UtilState::getLookup( GRID, PEN_WIDTH);
    double newThickness = otherState.getValue<double>( thicknessLookup );
    _setGridThickness( newThickness, &gridThicknessChanged );

    bool spacingChanged = false;
    double newSpacing = otherState.getValue<double>( SPACING );
    _setGridSpacing( newSpacing, &spacingChanged );

    bool transparencyChanged = false;
    double newTransparency = otherState.getValue<double>( TRANSPARENCY );
    _setGridTransparency( newTransparency, &transparencyChanged );

    bool fontFamilyChanged = false;
    QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
    QString newFamily = otherState.getValue<QString>( familyLookup );
    _setFontFamily( newFamily, &fontFamilyChanged );

    bool fontSizeChanged = false;
    QString sizeLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_SIZE );
    int fontSize = otherState.getValue<int>( sizeLookup );
    _setFontSize( fontSize, &fontSizeChanged );

    bool labelColorChanged = false;
    QPen labelPen = _getPen( LABEL_COLOR, otherState );
    QColor labelColor = labelPen.color();
    _setLabelColor( labelColor.red(), labelColor.green(), labelColor.blue(), &labelColorChanged );

    bool gridColorChanged = false;
    QPen gridPen = _getPen( GRID, otherState );
    QColor gridColor = gridPen.color();
    _setGridColor( gridColor.red(), gridColor.green(), gridColor.blue(), & gridColorChanged );

    bool axesColorChanged = false;
    QPen axesPen = _getPen( AXES, otherState );
    QColor axesColor = axesPen.color();
    _setAxesColor( axesColor.red(), axesColor.green(), axesColor.blue(), & axesColorChanged );

    stateChanged = internalChanged || coordChanged || axesChanged  ||
            gridLinesChanged || gridThicknessChanged || spacingChanged ||
            transparencyChanged || fontFamilyChanged || fontSizeChanged ||
            labelColorChanged || gridColorChanged || axesColorChanged;
    if ( stateChanged ){
        _resetGridRenderer();
    }
    return stateChanged;
}

QStringList DataGrid::_setAxesColor( int redAmount, int greenAmount, int blueAmount,
        bool* axesColorChanged ){
    QStringList result = _setColor( AXES, redAmount, greenAmount, blueAmount, axesColorChanged );
    return result;
}

QStringList DataGrid::_setColor( const QString& key, int redAmount, int greenAmount, int blueAmount,
        bool* colorChanged ){
    QStringList result;
    if ( redAmount < 0 || redAmount > 255 ){
        result.append("Red must be in [0,255]: "+QString::number( redAmount ));
    }
    if ( greenAmount < 0 || greenAmount > 255 ){
        result.append("Green must be in [0,255]: "+QString::number( greenAmount ));
    }
    if ( blueAmount < 0 || blueAmount > 255 ){
        result.append("Blue amount must be in [0,255]: "+QString::number( blueAmount ));
    }
    if ( result.size() == 0 ){
        *colorChanged = false;
        QString redLookup = Carta::State::UtilState::getLookup( key, RED );
        if ( m_state.getValue<int>(redLookup) != redAmount ){
            m_state.setValue<int>( redLookup, redAmount );
            *colorChanged = true;
        }
        QString blueLookup = Carta::State::UtilState::getLookup( key, BLUE );
        if ( m_state.getValue<int>(blueLookup) != blueAmount ){
            m_state.setValue<int>( blueLookup, blueAmount );
            *colorChanged = true;
        }
        QString greenLookup = Carta::State::UtilState::getLookup( key, GREEN );
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
    bool validCoord = m_coordSystems->isCoordinateSystem( coordSystem );
    if ( validCoord ){
        if ( m_state.getValue<QString>( COORD_SYSTEM) != coordSystem ){
            m_state.setValue<QString>( COORD_SYSTEM, coordSystem );
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
    bool validFont = m_fonts->isFontFamily( fontFamily );
    if ( validFont ){
        QString familyLookup = Carta::State::UtilState::getLookup( FONT, Fonts::FONT_FAMILY );
        if ( m_state.getValue<QString>( familyLookup ) != fontFamily ){
            m_state.setValue<QString>( familyLookup, fontFamily );
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

QString DataGrid::_setGridThickness( double thickness, bool* coordChanged ){
    *coordChanged = false;
    QString result;
    if ( thickness < 0 || thickness > 1 ){
        result = "Grid thickness must be in [0,1]: "+QString::number(thickness);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( GRID, PEN_WIDTH );
        double oldThickness = m_state.getValue<double>(lookup);
        if ( fabs( oldThickness - thickness ) > m_errorMargin ){
            m_state.setValue<double>( lookup, thickness);
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

QString DataGrid::_setGridTransparency( double transparency, bool* transparencyChanged ){
    *transparencyChanged = false;
    QString result;
    if ( transparency < 0 || transparency > 1 ){
        result = "Grid transparency must be in [0,1]: "+ QString::number( transparency );
    }
    else {
        double oldTransparency = m_state.getValue<double>(TRANSPARENCY);
        if ( fabs( oldTransparency - transparency ) > m_errorMargin ){
            m_state.setValue<double>( TRANSPARENCY, transparency);
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


DataGrid::~DataGrid(){

}
}
}
