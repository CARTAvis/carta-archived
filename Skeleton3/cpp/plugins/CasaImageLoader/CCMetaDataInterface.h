/**
 *
 **/

#pragma once

#include "common/misc.h"
#include "common/IImage.h"

class CCMetaDataInterface : public Image::MetaDataInterface
{
    CLASS_BOILERPLATE( CCMetaDataInterface );

public:
    CCMetaDataInterface(QString html);

    virtual Image::MetaDataInterface *
    clone() override;

    virtual CoordinateFormatterInterface *
    coordinateFormatter() override;

    virtual CoordinateGridPlotterInterface &
    coordinateGridPlotter() override;

    virtual PlotLabelGeneratorInterface &
    plotLabelGenerator() override;

    virtual QString
    title( TextFormat format ) override;

    virtual QStringList
    otherInfo( TextFormat format ) override;

protected:
    Carta::HtmlString m_title;
};
