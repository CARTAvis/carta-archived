/**
 *
 **/

#include "HtmlString.h"
#include <QTextDocumentFragment>

QString
Carta::Lib::HtmlString::html() const
{
    if ( m_html.isNull() ) {
        return m_plain.toHtmlEscaped();
    }
    else {
        return m_html;
    }
}

QString
Carta::Lib::HtmlString::plain() const
{
    if ( m_plain.isNull() ) {
        return QTextDocumentFragment::fromHtml( m_html ).toPlainText();
    }
    else {
        return m_plain;
    }
}

Carta::Lib::HtmlString
Carta::Lib::HtmlString::fromHtml( QString html )
{
    HtmlString res;
    res.m_html = html;
    return res;
}

Carta::Lib::HtmlString Carta::Lib::HtmlString::fromPlain(QString plain)
{
    HtmlString res;
    res.m_plain = plain;
    return res;
}

Carta::Lib::HtmlString::HtmlString( QString plain, QString html )
{
    m_plain = plain.isNull() ? "" : plain;
    m_html  = html.isNull() ? "" : html;
}

Carta::Lib::HtmlString::HtmlString() { }
