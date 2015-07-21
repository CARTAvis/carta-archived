/**
 *
 **/

#pragma once

#include <QString>

namespace Carta
{
namespace Lib
{
/// desired formatting
enum class TextFormat
{
    Plain, ///< request plain text version
    Html    ///< request html text version
};

/// holds a representation of a string in plain and html formats
class HtmlString
{
public:

    /// constructs an empty string (both texts will be set to "")
    HtmlString();

    /// constructs an instance with both plain and html texts set
    /// \param plain plain version of the text
    /// \param html html version of the text
    HtmlString( QString plain, QString html );

    /// constructs an instance with html text, plain text will be deduced
    /// by converting the html to plain text
    static HtmlString
    fromHtml( QString html );

    /// constructs an instance with plain text, html text will be deduced
    /// by escaping all html characters
    static HtmlString
    fromPlain( QString plain );

    /// returns the html text
    QString
    html() const;

    /// returns the plain text
    QString
    plain() const;

protected:

    QString m_html, m_plain;
};
}
} // Carta::Lib

