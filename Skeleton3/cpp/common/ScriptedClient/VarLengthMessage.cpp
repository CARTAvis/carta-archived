/**
 *
 **/

#include "VarLengthMessage.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
VarLengthSocket::VarLengthSocket( std::shared_ptr < QTcpSocket > rawSocket )
    : QObject( nullptr )
      , m_rawSocket( rawSocket )
{
    connect( m_rawSocket.get(), & QTcpSocket::readyRead,
             this, & VarLengthSocket::socketCB );
}

void
VarLengthSocket::send( const Carta::Core::ScriptedClient::VarLengthMessage & data )
{
    qint64 size = data.size();
    qint64 nsize = qToLittleEndian( size );
    sendNBytes( 8, & nsize );
    sendNBytes( size, data.constData() );
}

void
VarLengthSocket::socketCB()
{
    VarLengthMessage buff;
    qint64 size;
    readNBytes( 8, & size );
    size = qFromLittleEndian( size );
    buff.resize( size );
    readNBytes( size, buff.data() );

    emit received( buff );
}

void
VarLengthSocket::sendNBytes( qint64 n, const void * data )
{
    qint64 remaining = n;
    const char * ptr = (const char *) data;
    while ( remaining > 0 ) {
        auto written = m_rawSocket-> write( ptr, remaining );
        if ( written < 0 ) {
            throw std::runtime_error( "closed socket?" );
        }
        remaining -= written;
        ptr += written;
    }
}

void
VarLengthSocket::readNBytes( qint64 n, void * dest )
{
    qint64 remaining = n;
    char * ptr = reinterpret_cast < char * > ( dest );
    while ( remaining > 0 ) {
        auto n = m_rawSocket-> read( ptr, remaining );
        if ( n < 0 ) {
            throw std::runtime_error( "closed socket?" );
        }
        remaining -= n;
        ptr += n;
    }
}
}
}
}
