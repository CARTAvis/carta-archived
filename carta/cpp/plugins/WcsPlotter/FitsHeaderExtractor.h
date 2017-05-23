/**
 * Class encapsulating algorithms for extracting/computing a fits header from
 * a carta image.
 **/

#pragma once

#include "CartaLib/IImage.h"
#include <QStringList>

namespace casacore { class LatticeBase; }

class FitsHeaderExtractor
{
public:

    FitsHeaderExtractor();
    ~FitsHeaderExtractor();

    /// \brief Set the input image
    /// \param image pointer to the image from which to extract the header
    void setInput( Carta::Lib::Image::ImageInterface::SharedPtr image );

    /// \brief get the header
    /// \return the fits header, with 'END' included as last entry
    /// empty list indicates an error
    /// \note all entries are padded/truncated to 80 chars
    QStringList getHeader();

    /// \return list of errors (if any)
    QStringList getErrors();

private:

    QString _FITSKeyWordParser(QString _raw);
    QStringList _CasaFitsConverter( casacore::LatticeBase * lbase);

    Carta::Lib::Image::ImageInterface::SharedPtr m_cartaImage = nullptr;
    QStringList m_errors;
};

// FitsLine represents a single entry in the Fits header
struct FitsLine
{
    FitsLine( const QString & rawLine )
    {
        _raw = rawLine;
        _parse( _k, _v, _c );
    }

    /// return the raw line (verbatim)
    QString raw() { return _raw; }

    /// extract the keyword from the line
    QString key() { return _k; }

    /// extract a value from the line
    QString value() { return _v; }

    /// extract a comment from the line
    QString comment() {  return _c; }

    /// parse the line into key/value/comment
    /// \warning throws on syntax error!
    void _parse( QString & key, QString & value, QString & comment );

    static QString getline(QString & key, QString & value);

protected:

    QString _k, _v, _c, _raw;
};
