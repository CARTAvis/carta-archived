#include "ProfileFitThread.h"
#include "CartaLib/Hooks/Fit1DHook.h"
#include "CartaLib/Fit1DInfo.h"
#include "Globals.h"
#include "PluginManager.h"
#include <QDebug>

namespace Carta
{
namespace Data
{

ProfileFitThread::ProfileFitThread( QObject* parent ):
    QThread( parent ){
}

std::vector<Carta::Lib::Hooks::FitResult> ProfileFitThread::getResult() const {
    return m_results;
}


void ProfileFitThread::run(){
    m_results.clear();
    int dataCount = m_fitInfos.size();
    for ( int i = 0; i < dataCount; i++ ){
        auto result = Globals::instance()-> pluginManager()
                                                 -> prepare <Carta::Lib::Hooks::Fit1DHook>( m_fitInfos[i]);
        auto lam = [=] ( const Carta::Lib::Hooks::Fit1DHook::ResultType &result ) {
            m_results.push_back( result );
        };
        try {
            result.forEach( lam );
        }
        catch( char*& error ){
            qDebug() << "Error: "<<error;
        }
    }
}

void ProfileFitThread::setParams( const std::vector<Carta::Lib::Fit1DInfo>& fitInfos ){
    m_fitInfos = fitInfos;
}


ProfileFitThread::~ProfileFitThread(){
}
}
}

