#include "ImageView.h"
#include "Globals.h"
#include "IPlatform.h"
#include "IConnector.h"
#include "PluginManager.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"

#include <iostream>

#include <QPainter>
#include <cmath>
#include <QDebug>
#include <QCoreApplication>

const QString ImageView::MOUSE = "mouse";
const QString ImageView::VIEW = "view";
const QString ImageView::MOUSE_Y = "mouse/x";
const QString ImageView::MOUSE_X = "mouse/y";

ImageView::ImageView(const QString & viewName, QColor bgColor, QImage img,
        Carta::State::StateInterface* mouseState){
    m_defaultImage = img;
    m_qimage = QImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    m_qimage.fill(bgColor);

    m_viewName = Carta::State::UtilState::getLookup( viewName, VIEW);
    m_connector = nullptr;
    m_bgColor = bgColor;
    m_mouseState = mouseState;
}

void ImageView::resetImage(QImage img) {
     m_defaultImage = img;
}

void ImageView::registration(IConnector *connector) {
    m_connector = connector;
}

const QString & ImageView::name() const {
    return m_viewName;
}

QSize ImageView::size() {
    return m_qimage.size();
}

const QImage & ImageView::getBuffer() {
    redrawBuffer();
    return m_qimage;
}

void ImageView::handleResizeRequest(const QSize & size) {
    if ( size.height() > 0 && size.width() > 0 ){
        m_qimage = QImage(size, m_qimage.format());
        emit resize( size );
    }
}

void ImageView::handleMouseEvent(const QMouseEvent & ev) {
    m_lastMouse = QPointF(ev.x(), ev.y());
    m_mouseState->setValue<QString>( MOUSE_X, QString::number(ev.x()));
    m_mouseState->setValue<QString>( MOUSE_Y, QString::number(ev.y()));
    m_mouseState->flushState();
}


void ImageView::scheduleRedraw(){
    redrawBuffer();
    m_connector-> refreshView( this );
}

void ImageView::redrawBuffer() {
    if ( !m_qimage.isNull() ){
        m_qimage.fill( "#E0E0E0" );
        {
            QPainter p(&m_qimage);
            p.drawImage(m_qimage.rect(), m_defaultImage);
        }

//        // execute the pre-render hook
//        Globals::instance()->pluginManager()->prepare<PreRender>(m_viewName,
//                &m_qimage).executeAll();
    }
}


