#include "TestView.h"
#include "Globals.h"
#include "IPlatform.h"
#include "IConnector.h"
#include "PluginManager.h"
//#include "Data/DataController.h"
#include <iostream>
//#include <QImage>
//#include <QColor>
#include <QPainter>
#include <cmath>
#include <QDebug>
#include <QCoreApplication>



TestView::TestView(const QString & viewName, QColor bgColor, QImage img) {
    m_defaultImage = img;
    m_qimage = QImage(100, 100, QImage::Format_RGB888);
    m_qimage.fill(bgColor);

    m_viewName = viewName;
    m_connector = nullptr;
    m_bgColor = bgColor;
}

void TestView::resetImage(QImage img) {
     m_defaultImage = img;
}

void TestView::registration(IConnector *connector) {
    m_connector = connector;
}

const QString & TestView::name() const {
    return m_viewName;
}

QSize TestView::size() {
    return m_qimage.size();
}

const QImage & TestView::getBuffer() {
    redrawBuffer();
    return m_qimage;
}

void TestView::handleResizeRequest(const QSize & size) {
    m_qimage = QImage(size, m_qimage.format());
    m_connector->refreshView(this);
}

void TestView::handleMouseEvent(const QMouseEvent & ev) {
    m_lastMouse = QPointF(ev.x(), ev.y());
    m_connector->refreshView(this);

    m_connector->setState(StateKey::MOUSE_X, m_viewName,
                QString::number(ev.x()));
    m_connector->setState(StateKey::MOUSE_Y, m_viewName,
                QString::number(ev.y()));
}

void TestView::handleKeyEvent(const QKeyEvent & /*event*/) {
}

void TestView::redrawBuffer() {
    QPointF center = m_qimage.rect().center();
    QPointF diff = m_lastMouse - center;
    double angle = atan2(diff.x(), diff.y());
    angle *= -180 / M_PI;

    m_qimage.fill(m_bgColor);
    {
        QPainter p(&m_qimage);
        p.drawImage(m_qimage.rect(), m_defaultImage);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 0, 128));
        p.drawEllipse(QPoint(m_lastMouse.x(), m_lastMouse.y()), 10, 10);
        p.setPen(QColor(255, 255, 255));
        p.drawLine(0, m_lastMouse.y(), m_qimage.width() - 1,
                m_lastMouse.y());
        p.drawLine(m_lastMouse.x(), 0, m_lastMouse.x(),
                m_qimage.height() - 1);

        p.translate(m_qimage.rect().center());
        p.rotate(angle);
        p.translate(-m_qimage.rect().center());
        p.setFont(QFont("Arial", 20));
        p.setPen(QColor("white"));
        p.drawText(m_qimage.rect(), Qt::AlignCenter, m_viewName);
    }

    // execute the pre-render hook
    Globals::instance()->pluginManager()->prepare<PreRender>(m_viewName,
            &m_qimage).executeAll();
}


