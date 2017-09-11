#include <math.h>
#include "DataContours.h"
#include "GeneratorState.h"
#include "Contour.h"

#include "Data/Util.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataContours::CLASS_NAME = "DataContours";
const QString DataContours::CONTOURS = "contours";
const QString DataContours::CONTOUR_DRAW = "contourDraw";



class DataContours::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new DataContours (path, id);
        }
    };

bool DataContours::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new DataContours::Factory());

DataContours::DataContours( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_generatorState( new GeneratorState() ) {
    _initializeDefaultState();
}

std::set<Contour> DataContours::_getContours(){
    return m_contours;
}

Contour* DataContours::_getContour(double level) {
    Contour* target = nullptr;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); it++ ){
        double existingLevel = (*it).getLevel();
        if ( qAbs( existingLevel - level ) < Contour::ERROR_MARGIN ){
            target = const_cast<Contour*> (&( *it ));
            break;
        }
    }
    return target;
}

std::shared_ptr<GeneratorState> DataContours::_getGenerator() const {
    return m_generatorState;
}

int DataContours::getLevelCount() const {
    return m_contours.size();
}

std::vector<double> DataContours::getLevels() const {
    int levelCount = m_contours.size();
    std::vector<double> levels( levelCount );
    int i = 0;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                    it != m_contours.end(); it++ ){
        levels[i] = (*it).getLevel();
        i++;
    }
    return levels;
}

QString DataContours::getName() const {
    return m_state.getValue<QString>( Util::NAME );
}

QString DataContours::getContourType() const {
    return m_generatorState -> getContourType();
}

std::vector<QPen> DataContours::getPens() const {
    int penCount = m_contours.size();
    std::vector<QPen> pens( penCount );
    int i = 0;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                it != m_contours.end(); it++ ){
        pens[i] = (*it).getPen();
        i++;
    }
    return pens;
}


Carta::State::StateInterface DataContours::_getState() const {
    return m_state;
}


void DataContours::_initializeCallbacks(){

}

void DataContours::_initializeDefaultState(){

    int contourCount = m_contours.size();
    m_state.insertArray( CONTOURS, contourCount );
    m_state.insertValue<bool>( CONTOUR_DRAW, true );
    m_state.insertValue<QString>( Util::NAME, "");

    m_state.flushState();
}



bool DataContours::isContourDraw() const {
    return m_state.getValue<bool>( CONTOUR_DRAW );
}

bool DataContours::operator<( const DataContours& other ) const {
    bool lessThan = false;
    int compareResult = QString::compare( getName(), other.getName(), Qt::CaseInsensitive );
    if ( compareResult < 0 ){
        lessThan = true;
    }
    return lessThan;
}

void DataContours::resetState( const QString& state ){
    //Update the state, then set the contours to match.
    CartaObject::resetState( state );
    int levelCount = m_state.getArraySize( CONTOURS);
    m_contours.clear();
    for ( int i = 0; i < levelCount; i++){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        QString contourState = m_state.toString( indexLookup );
        Contour contour;
        contour.setStateString( contourState );
        m_contours.insert( contour );
    }
    m_state.resizeArray( CONTOURS, m_contours.size() );
    _updateContourState();
}

QString DataContours::setAlpha( std::vector<double>& levels, int transparency ){
    QString result;
    int levelCount = levels.size();
    bool alphaSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setAlpha( transparency, &alphaSet );
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( alphaSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QStringList DataContours::setColor( std::vector<double>& levels, int red, int green, int blue ){
    QStringList result;
    int levelCount = levels.size();
    bool colorSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setColor( red, green, blue, &colorSet );
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( colorSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result.append( "Unrecognized contour levels: "+unrecognizedLevels);
    }
    return result;
}

void DataContours::setContours( std::set<Contour>& contours ){
    int levelCount = contours.size();
    m_state.resizeArray( CONTOURS, levelCount );
    m_contours.clear();
    int i = 0;
    for ( std::set<Contour>::iterator it = contours.begin();
                    it != contours.end(); it++ ){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        m_state.setObject( indexLookup, (*it).getStateString());
        m_contours.insert( (*it) );
        i++;
    }
}

bool DataContours::setLevels( std::vector<double>& levels ){
    int levelCount = levels.size();
    bool levelsChanged = false;
    //First go through the levels and make sure there is a contour
    //for each level; add a contour if there is not already one there.
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel == nullptr ){
            Contour contour;
            contour.setLevel( levels[i], &levelsChanged );
            m_contours.insert( contour );
        }
    }

    //Now iterate through the contour levels and remove any that do not match
    //one of the levels passed in.
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); ){
        double contourLevel = (*it).getLevel();
        bool levelFound = false;
        for ( int i = 0; i < levelCount; i++ ){
            if ( qAbs( contourLevel - levels[i] ) < Contour::ERROR_MARGIN ){
                levelFound = true;
                break;
            }
        }
        if ( !levelFound ){
            levelsChanged = true;
            m_contours.erase( it++ );
        }
        else {
            it++;
        }
    }
    if ( levels.size() != m_contours.size() ){
        levelsChanged = true;
    }

    if ( levelsChanged ){
        m_state.resizeArray( CONTOURS, m_contours.size() );
        _updateContourState();
    }
    return levelsChanged;
}


QString DataContours::setLineStyle( std::vector<double>& levels, const QString& lineStyle ){
    QString result;
    int levelCount = levels.size();
    bool lineStyleSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setStyle( lineStyle, &lineStyleSet );
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( lineStyleSet ){
        _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
        result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QString DataContours::setName( const QString& name ){
    QString result;
    if ( !name.isEmpty() && name.length() > 0 ){
        QString oldName = m_state.getValue<QString>( Util::NAME );
        if ( oldName != name ){
            m_state.setValue<QString>( Util::NAME, name );
        }
    }
    else {
        result = "Please choose a nontrivial name for the contour set.";
    }
    return result;
}

QString DataContours::setThickness( std::vector<double>& levels, double thickness ){
    QString result;
    int levelCount = levels.size();
    bool thicknessSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setWidth( thickness, &thicknessSet );
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( thicknessSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QString DataContours::setVisibility( std::vector<double>& levels, bool visible ){
    QString result;
    int levelCount = levels.size();
    bool visibilitySet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            bool visibilityChanged = contourLevel->setVisible( visible );
            if ( visibilityChanged ){
                visibilitySet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( visibilitySet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

void DataContours::_updateContourState( ){
    int i = 0;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); it++ ){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        m_state.setObject( indexLookup, (*it).getStateString());
        i++;
    }
    m_state.flushState();
}

void DataContours::_updateGeneratorState( const std::shared_ptr<GeneratorState>& other ){
    m_generatorState->_updateState( other );
}

DataContours::~DataContours(){

}
}
}
