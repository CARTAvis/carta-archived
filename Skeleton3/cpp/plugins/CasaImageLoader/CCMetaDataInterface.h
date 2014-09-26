/**
 *
 **/

#pragma once

#include "common/IImage.h"

class CCMetaDataInterface : public Image::MetaDataInterface
{
public:
    CCMetaDataInterface();

    // MetaDataInterface interface
public:
    virtual Image::MetaDataInterface *clone() override;
    virtual CoordinateFormatterInterface *coordinateFormatter() override;
    virtual CoordinateGridPlotterInterface &coordinateGridPlotter() override;
    virtual PlotLabelGeneratorInterface &plotLabelGenerator() override;
    virtual QString title(TextFormat format) override;
    virtual QStringList otherInfo(TextFormat format) override;
};

