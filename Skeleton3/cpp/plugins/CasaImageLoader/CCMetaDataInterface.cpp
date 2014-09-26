/**
 *
 **/


#include "CCMetaDataInterface.h"

CCMetaDataInterface::CCMetaDataInterface()
{
}


Image::MetaDataInterface *CCMetaDataInterface::clone()
{
    qFatal( "not implemented");
}

CoordinateFormatterInterface *CCMetaDataInterface::coordinateFormatter()
{
    qFatal( "not implemented");
}

CoordinateGridPlotterInterface &CCMetaDataInterface::coordinateGridPlotter()
{
    qFatal( "not implemented");
}

PlotLabelGeneratorInterface &CCMetaDataInterface::plotLabelGenerator()
{
    qFatal( "not implemented");
}

QString CCMetaDataInterface::title(TextFormat format)
{
    Q_UNUSED( format);
    qFatal( "not implemented");
}

QStringList CCMetaDataInterface::otherInfo(TextFormat format)
{
    Q_UNUSED( format);
    qFatal( "not implemented");
}
