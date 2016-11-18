
#include "Data/Image/Render/RenderRequest.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {


RenderRequest::RenderRequest( const std::vector<int>& frames,
        const Carta::Lib::KnownSkyCS& cs ){
    m_frames = frames;
    m_cs = cs;
    m_stackTop = false;
    m_topIndex = -1;
    m_zoom = -1;
    m_requestMain = false;
    m_requestContext = false;
    m_requestZoom = false;
    m_minClipPercent = 0.025;
    m_maxClipPercent = 0.975;
    m_recomputeClips = true;
    m_pan = QPointF( nan(""), nan(""));
}

double RenderRequest::getClipPercentMin() const {
	return m_minClipPercent;
}

double RenderRequest::getClipPercentMax() const {
	return m_maxClipPercent;
}

std::vector<int> RenderRequest::getFrames() const {
    return m_frames;
}


Carta::Lib::KnownSkyCS RenderRequest::getCoordinateSystem() const {
    return m_cs;
}

QList<std::shared_ptr<Layer> > RenderRequest::getData() const {
    return m_datas;
}



double RenderRequest::getZoom() const {
    return m_zoom;
}


QPointF RenderRequest::getPan() const {
    return m_pan;
}


QSize RenderRequest::getOutputSize() const {
    return m_outputSize;
}


int RenderRequest::getTopIndex() const {
    return m_topIndex;
}

bool RenderRequest::isRecomputeClips() const {
	return m_recomputeClips;
}

bool RenderRequest::isStackTop() const {
    return m_stackTop;
}


bool RenderRequest::isPanSet() const {
    bool panSet = false;
    if ( !std::isnan( m_pan.x() ) && !std::isnan( m_pan.y() ) ){
        panSet = true;
    }
    return panSet;
}

/*bool RenderRequest::isRecomputeClips() const {
	return m_recomputeClips;
}*/

bool RenderRequest::isRequestContext() const {
    return m_requestContext;
}

bool RenderRequest::isRequestMain() const {
    return m_requestMain;
}


bool RenderRequest::isRequestZoom() const {
    return m_requestZoom;
}


bool RenderRequest::isZoomSet() const {
    bool zoomSet = false;
    if ( m_zoom > 0 ){
        zoomSet = true;
    }
    return zoomSet;
}

bool RenderRequest::operator==( const RenderRequest& other ) const {
	bool equalRequests = false;
	const double ERROR_MARGIN = 1;
	if ( other.getFrames() == m_frames ){
		if ( other.getCoordinateSystem() == m_cs ){
			if ( other.getTopIndex()  == m_topIndex ){
				if ( other.isStackTop() == m_stackTop ){
					if ( other.getZoom()  == m_zoom ){
						if ( other.isRequestMain()  == m_requestMain ){
							if ( other.isRequestContext() == m_requestContext ){
								if ( other.isRequestZoom()  == m_requestZoom ){
									QPointF otherPan = other.getPan();
									double dX = qAbs( otherPan.x() - m_pan.x());
									double dY = qAbs( otherPan.y() - m_pan.y());
									if ( (dX<ERROR_MARGIN && dY< ERROR_MARGIN) ||
											//Both x  and y are nans
											( otherPan.x() != otherPan.x() && m_pan.x() != m_pan.x() &&
													otherPan.y()!= otherPan.y() && m_pan.y()!= m_pan.y() )){
										QList<std::shared_ptr<Layer> > otherData = other.getData();
										if ( otherData.size() == m_datas.size()){
											//We could put in a check to make sure the layers are the
											//same, but this is a quick check that reduces the size of the
											//queue.
											equalRequests = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return equalRequests;
}

void RenderRequest::setData( QList<std::shared_ptr<Layer> > datas ){
    m_datas = datas;
}


void RenderRequest::setOutputSize( const QSize& size ){
    m_outputSize = size;
}


void RenderRequest::setPan( QPointF pan ){
    m_pan = pan;
}


void RenderRequest::setRequestMain( bool mainRequest ){
    m_requestMain = mainRequest;
}


void RenderRequest::setRequestContext( bool contextRequest ){
    m_requestContext = contextRequest;
}


void RenderRequest::setRequestZoom( bool zoomRequest ){
    m_requestZoom = zoomRequest;
}


void RenderRequest::setStackTop( bool stackTop ){
    m_stackTop = stackTop;
}


void RenderRequest::setTopIndex( int topIndex ){
    m_topIndex = topIndex;
}


void RenderRequest::setZoom( double zoomValue ){
    CARTA_ASSERT( zoomValue > 0);
    m_zoom = zoomValue;
}

void RenderRequest::setRecomputeClips( bool recomputeClipsOnNewFrame ){
	m_recomputeClips = recomputeClipsOnNewFrame;
}

void RenderRequest::setClipPercents( double minClipPercent, double maxClipPercent ){
	m_minClipPercent = minClipPercent;
	m_maxClipPercent = maxClipPercent;
}

RenderRequest::~RenderRequest(){

}
}
}
