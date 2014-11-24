/**
 *
 **/

#include "HtmlString.h"
#include <QTextDocumentFragment>

namespace Carta
{
namespace Lib
{

QString
HtmlString::html() const
{
    if ( m_html.isNull() ) {
        return m_plain.toHtmlEscaped();
    }
    else {
        return m_html;
    }
}

QString
HtmlString::plain() const
{
    if ( m_plain.isNull() ) {
        return QTextDocumentFragment::fromHtml( m_html ).toPlainText();
    }
    else {
        return m_plain;
    }
}

HtmlString
HtmlString::fromHtml( QString html )
{
    HtmlString res;
    res.m_html = html;
    return res;
}

HtmlString HtmlString::fromPlain(QString plain)
{
    HtmlString res;
    res.m_plain = plain;
    return res;
}

HtmlString::HtmlString( QString plain, QString html )
{
    m_plain = plain.isNull() ? "" : plain;
    m_html  = html.isNull() ? "" : html;
}

HtmlString::HtmlString() { }

}
}
