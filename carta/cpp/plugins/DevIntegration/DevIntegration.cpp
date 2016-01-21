#include "DevIntegration.h"
#include "CartaLib/Hooks/GetInitialFileList.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

typedef Carta::Lib::Hooks::Initialize Initialize;

static const char * PluginName = "DevIntegrationPlugin:";

DevIntegrationPlugin::DevIntegrationPlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
DevIntegrationPlugin::handleHook( BaseHook & hookData )
{
    qDebug() << PluginName << "handling hook #" << hookData.hookId();
    if ( hookData.is < Initialize > () ) {
        // we may need to initialize some qt formats here in the future...
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::GetInitialFileList > () ) {
        if ( ! m_enabled ) {
            return false;
        }
        Carta::Lib::Hooks::GetInitialFileList & hook =
            static_cast < Carta::Lib::Hooks::GetInitialFileList & > ( hookData );
        QString file = hook.paramsPtr->urlParams["file"];
        qDebug() << PluginName << "file" << file;
        if ( file.isEmpty() ) {
            hook.result.clear();
        }
        else {
            hook.result = {
                file
            };
        }

        // return success if we made a list of at least one filename
        return ! hook.result.isEmpty();
    }

    qWarning() << PluginName << "Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
DevIntegrationPlugin::getInitialHookList()
{
    return {
               Initialize::staticId,
               Carta::Lib::Hooks::GetInitialFileList::staticId
    };
}

void
DevIntegrationPlugin::initialize( const IPlugin::InitInfo & info )
{
    qDebug() << PluginName << "initialized";

    m_enabled = info.json.value( "enabled" ).toBool( false );

    qDebug() << PluginName << "enabled=" << m_enabled;
}
