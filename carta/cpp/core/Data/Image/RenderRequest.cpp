
#include "Data/Image/RenderRequest.h"
#include <QDebug>

namespace Carta {

namespace Data {


RenderRequest::RenderRequest( const std::vector<int>& frames,
        const Carta::Lib::KnownSkyCS& cs, bool topOfStack,
        const QSize& outputSize ){
    m_frames = frames;
    m_cs = cs;
    m_stackTop = topOfStack;
    m_outputSize = outputSize;
    m_topIndex = -1;
}


std::vector<int> RenderRequest::getFrames() const {
    return m_frames;
}

Carta::Lib::KnownSkyCS RenderRequest::getCoordinateSystem() const {
    return m_cs;
}

QSize RenderRequest::getOutputSize() const {
    return m_outputSize;
}

int RenderRequest::getTopIndex() const {
    return m_topIndex;
}

bool RenderRequest::isStackTop() const {
    return m_stackTop;
}


void RenderRequest::setTopIndex( int topIndex ){
    m_topIndex = topIndex;
}


RenderRequest::~RenderRequest(){

}

}
}
