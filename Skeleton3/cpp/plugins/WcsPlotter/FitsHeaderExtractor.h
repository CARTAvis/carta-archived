/**
 * Class encapsulating algorithms for extracting/computing a fits header from
 * a carta image.
 **/

#pragma once

#include "CartaLib/IImage.h"
#include <QStringList>

namespace casa { class LatticeBase; }

class FitsHeaderExtractor
{
public:

    FitsHeaderExtractor();
    ~FitsHeaderExtractor();

    /// \brief Set the input image
    /// \param image pointer to the image from which to extract the header
    void
    setInput( Image::ImageInterface::SharedPtr image );

    /// \brief get the header
    /// \return the fits header, with 'END' included as last entry
    /// empty list indicates an error
    /// \note all entries are padded/truncated to 80 chars
    QStringList
    getHeader();

    /// \return list of errors (if any)
    QStringList
    getErrors();

private:

    QStringList tryRawFits( QString fname);
    QStringList tryCasaCoreFitsConverter( casa::LatticeBase * lbase);

    Image::ImageInterface::SharedPtr m_cartaImage = nullptr;
    QStringList m_errors;
};
