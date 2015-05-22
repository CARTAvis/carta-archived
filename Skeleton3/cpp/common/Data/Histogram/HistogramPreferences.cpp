#include "HistogramPreferences.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {


const QString HistogramPreferences::CLASS_NAME = "HistogramPreferences";
const QString HistogramPreferences::PREFERENCES = "Preferences";
const QString HistogramPreferences::HIST_BIN_COUNT = "histogramBinCount";
const QString HistogramPreferences::HIST_CLIPS = "histogramClips";
const QString HistogramPreferences::HIST_CUBE = "histogramCube";
const QString HistogramPreferences::HIST_2D = "histogram2D";
const QString HistogramPreferences::HIST_RANGE = "histogramRange";
const QString HistogramPreferences::HIST_DISPLAY = "histogramDisplay";
const QString HistogramPreferences::VISIBLE = "visible";

using Carta::State::UtilState;

class HistogramPreferences::Factory : public Carta::State::CartaObjectFactory {
    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new HistogramPreferences (path, id);
        }
    };

bool HistogramPreferences::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                new HistogramPreferences::Factory());

HistogramPreferences::HistogramPreferences( const QString& path, const QString& id):
    CartaObject( CLASS_NAME,path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

QString HistogramPreferences::getStateString( const QString& /*sessionId*/,
        SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}

void HistogramPreferences::_initializeDefaultState(){

    m_state.insertValue<bool>( HIST_BIN_COUNT, false );
    m_state.insertValue<bool>( HIST_CLIPS, false );
    m_state.insertValue<bool>( HIST_CUBE, false );
    m_state.insertValue<bool>( HIST_2D, false );
    m_state.insertValue<bool>( HIST_RANGE, false );
    m_state.insertValue<bool>( HIST_DISPLAY, false );
    m_state.flushState();
}

void HistogramPreferences::_initializeCallbacks(){
    addCommandCallback( "setVisibleHistogramBinCount", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogramBinCount( visible );
               }
               else {
                   result = "Histogram bin count setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleHistogramClips", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogramClips( visible );
               }
               else {
                   result = "Histogram clip setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleHistogramCube", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogramCube( visible );
               }
               else {
                   result = "Histogram cube setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleHistogram2D", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogram2D( visible );
               }
               else {
                   result = "Histogram two dimensional setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleHistogramRange", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogramRange( visible );
               }
               else {
                   result = "Histogram range setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleHistogramDisplay", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleHistogramDisplay( visible );
               }
               else {
                   result = "Histogram display setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });


}

bool HistogramPreferences::_processParams( const QString& params, bool* value ) const {
    bool validValue = false;
    std::set<QString> keys = {VISIBLE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    *value = Util::toBool( dataValues[*keys.begin()], &validValue );
    return validValue;
}

void HistogramPreferences::_setVisibility( const QString& key, bool visible ){
    if ( m_state.getValue<bool>(key) != visible ){
        m_state.setValue<bool>(key, visible );
        m_state.flushState();
    }
}



void HistogramPreferences::setVisibleHistogramClips( bool visible ){
    _setVisibility( HIST_CLIPS, visible );
}

void HistogramPreferences::setVisibleHistogramBinCount( bool visible ){
    _setVisibility( HIST_BIN_COUNT, visible );
}

void HistogramPreferences::setVisibleHistogramCube( bool visible ){
    _setVisibility( HIST_CUBE, visible );
}

void HistogramPreferences::setVisibleHistogram2D( bool visible ){
    _setVisibility( HIST_2D, visible );
}

void HistogramPreferences::setVisibleHistogramRange( bool visible ){
    _setVisibility( HIST_RANGE, visible );
}

void HistogramPreferences::setVisibleHistogramDisplay( bool visible ){
    _setVisibility( HIST_DISPLAY, visible );
}

HistogramPreferences::~HistogramPreferences(){

}
}
}
