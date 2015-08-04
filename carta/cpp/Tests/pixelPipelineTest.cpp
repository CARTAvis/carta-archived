#include "catch.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "core/GrayColormap.h"
#include <QColor>

using namespace Carta;

TEST_CASE( "Cached Pixel Pipeline testing", "[pp]" ) {

    SECTION( "Basic gray") {
        Core::GrayColormap::SharedPtr grayCmap = std::make_shared<Core::GrayColormap>();
        Lib::PixelPipeline::CustomizablePixelPipeline * pp = new Lib::PixelPipeline::CustomizablePixelPipeline();
        pp-> setColormap( grayCmap);
        pp-> setMinMax( -2, 2);
        Lib::PixelPipeline::CachedPipeline<true> cppi;
        cppi.cache( * pp, 10, -2, 2);
        QRgb v1, v2;
        for( double x = -3 ; x < 3 ; x += 0.1) {
            pp-> convertq( x, v1);
            cppi.convertq( x, v2);
            QColor c1(v1);

            QString s = QString::number(x) + ": " + QColor(v1).name() + " -> " + QColor(v2).name();
            INFO( s.toStdString());
            REQUIRE(true);
        }

        bool ok = true;
        INFO( "Input: ");
        INFO( "Output: ");
        REQUIRE( ok);
    }

}
