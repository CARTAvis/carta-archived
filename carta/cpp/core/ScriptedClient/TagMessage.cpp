/**
 *
 **/

#include "TagMessage.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
TagMessage::TagMessage( QString tag, const VarLengthMessage & data ) :
    m_tag( tag ), m_data( data )
{
    ;
}

const QString &
TagMessage::tag() const
{
    return m_tag;
}

VarLengthMessage
TagMessage::toVarLengthMessage() const
{
    VarLengthMessage buff;
    buff.append( tag() );
    buff.append( '\0' );
    buff.append( data() );
    return buff;
}

TagMessage
TagMessage::fromVarLengthMessage( const QByteArray & data )
{
    QString tag = data.constData();
    VarLengthMessage ba = data.right( data.size() - tag.size() - 1 );
    return TagMessage( tag, ba );
}

TagMessageSocket::TagMessageSocket( std::shared_ptr < QTcpSocket > rawSocket )
    : QObject( nullptr )
{
    m_vlSocket.reset( new VarLengthSocket( rawSocket ) );
    connect( m_vlSocket.get(), & VarLengthSocket::received,
             this, & TagMessageSocket::vlenMessageCB );
}

void
TagMessageSocket::send( const TagMessage & message )
{
    // serialize and send the message
    m_vlSocket-> send( message.toVarLengthMessage() );
}

void
TagMessageSocket::vlenMessageCB( VarLengthMessage data )
{
    emit received( TagMessage::fromVarLengthMessage( data ) );
}
}
}
}
