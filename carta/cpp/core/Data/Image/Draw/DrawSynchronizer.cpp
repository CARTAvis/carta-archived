#include "DrawSynchronizer.h"
#include "ImageRenderService.h"
#include "CartaLib/IWcsGridRenderService.h"
#include "CartaLib/IContourGeneratorService.h"
#include "DefaultContourGeneratorService.h"
#include "Data/Image/Contour/DataContours.h"
#include <QDebug>

namespace Carta {

namespace Data {

DrawSynchronizer::DrawSynchronizer( std::shared_ptr<Carta::Core::ImageRenderService::Service> imageRendererService,
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridRendererService,
            QObject* parent)
        : QObject( parent ),
          m_irsJobId(-1),
          m_grsJobId(-1),
          m_cecJobId(-1),
          m_jobId(-1),

          m_irs( nullptr ),
          m_grs( nullptr ),
          m_cec( new Carta::Core::DefaultContourGeneratorService( this ) ){

    if ( ! connect( imageRendererService.get(), & Carta::Core::ImageRenderService::Service::done,
            this, & DrawSynchronizer::_irsDone ) ) {
        qCritical() << "Could not connect imageRenderService done slot";
    }
    if ( ! connect( gridRendererService.get(), & Carta::Lib::IWcsGridRenderService::done,
            this, & DrawSynchronizer::_wcsGridDone ) ) {
        qCritical() << "Could not connect gridRenderService done slot";
    }
    if ( ! connect( m_cec.get(), & Carta::Lib::IContourGeneratorService::done,
            this, & DrawSynchronizer::_contourDone ) ) {
        qCritical() << "Could not connect contour editor done slot";
    }

    m_irs = imageRendererService;
    m_grs = gridRendererService;
}

void DrawSynchronizer::_checkAndEmit(){
    // emit done only if all three are finished
    if ( m_grsDone && m_irsDone && m_cecDone ) {
        emit done( m_irsImage, m_grsVGList, m_cecVGList, m_regionVGList, m_jobId );
    }
}

void DrawSynchronizer::_contourDone( const Result & result,
            int64_t jobId){
    // if this is not the expected job, do nothing
    if ( jobId  == m_cecJobId ) {
        if( m_pens.size() != result.contours().size()) {
            qCritical() << "contour set entries:" << result.contours().size()
                        << "but pen entries:" << m_pens.size();
            return;
        }

        // convert the raw contours into VG
        Carta::Lib::VectorGraphics::VGComposer vgc;
        const auto & contourSet = result.contours();
        for ( size_t k = 0 ; k < contourSet.size() ; ++k ) {
            const auto & con = contourSet[k].polylines();
            vgc.append< Carta::Lib::VectorGraphics::Entries::SetPen >( m_pens[k]);
            for ( size_t i = 0 ; i < con.size() ; ++i ) {
                const QPolygonF & poly = con[i];
                vgc.append < Carta::Lib::VectorGraphics::Entries::DrawPolyline > ( poly );
            }
        }
        m_cecVGList = vgc.vgList();
        m_cecDone = true;
        _checkAndEmit();
    }
}

void DrawSynchronizer::_irsDone( QImage img, int64_t jobId ){
    // if this is not the expected job, do nothing
    if ( jobId == m_irsJobId ) {
        m_irsDone = true;
        m_irsImage = img;
        _checkAndEmit();
    }
}

void DrawSynchronizer::setInput( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> rawView ){
    m_cec->setInput( rawView );
}


void DrawSynchronizer::setContours( const std::set<std::shared_ptr<DataContours> > & contours ){
    std::vector<double> levels;
    bool drawing = false;
    m_pens.clear();
    for( std::set< std::shared_ptr<DataContours> >::iterator it = contours.begin();
            it != contours.end(); it++ ){
        if ( (*it)->isContourDraw() ){
            drawing = true;
            std::vector<QPen> setPens = (*it)->getPens();
            m_pens.insert( m_pens.end(), setPens.begin(), setPens.end());
            std::vector<double> setLevels = (*it)->getLevels();
            levels.insert( levels.end(), setLevels.begin(), setLevels.end());
        }
    }
    if ( drawing ){
        m_cec->setLevels( levels );
    }
}

void DrawSynchronizer::setRegionGraphics( const Carta::Lib::VectorGraphics::VGList& regionVGList ){
	m_regionVGList = regionVGList;
}

int64_t DrawSynchronizer::start( bool contourDraw, bool gridDraw, int64_t jobId ){
    m_irsDone = false;
    m_grsDone = !gridDraw;
    m_cecDone = !contourDraw;

    m_irsJobId = m_irs-> render();
    if ( jobId < 0 ) {
        m_jobId++;
    }
    else {
        m_jobId = jobId;
    }
    if ( gridDraw ){
        m_grsJobId = m_grs-> startRendering();
        m_jobId++;
    }
    else {
        //Empty the grid graphics list
        Carta::Lib::VectorGraphics::VGList emptyList;
        m_grsVGList = emptyList;
    }
    if ( contourDraw ){
        m_cecJobId = m_cec->start();
        m_jobId++;
    }
    else {
        //Empty the contour list
        Carta::Lib::VectorGraphics::VGList emptyList;
        m_cecVGList = emptyList;
    }
    return m_jobId;
}

void DrawSynchronizer::_wcsGridDone( Carta::Lib::VectorGraphics::VGList vgList,
             int64_t jobId ){
    // if this is not the expected job, do nothing
    if ( jobId == m_grsJobId ) {
        m_grsDone = true;
        m_grsVGList = vgList;
        _checkAndEmit();
    }
}

DrawSynchronizer::~DrawSynchronizer(){

}
}
}
