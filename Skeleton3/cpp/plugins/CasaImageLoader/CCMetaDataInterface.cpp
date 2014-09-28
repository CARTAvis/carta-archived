/**
 *
 **/


#include "CCMetaDataInterface.h"

CCMetaDataInterface::CCMetaDataInterface(QString html)
{
    m_title = Carta::HtmlString::fromHtml( html);
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
    if( format == TextFormat::Plain) {
        return m_title.plain();
    }
    else {
        return m_title.html();
    }

}

QStringList CCMetaDataInterface::otherInfo(TextFormat format)
{
    Q_UNUSED( format);
    qFatal( "not implemented");
}
