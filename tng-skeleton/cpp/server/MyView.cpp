#include "MyView.h"
#include <iostream>
#include <QColor>
#include <QPainter>
#include <cmath>

MyView::MyView(QObject *parent) : QObject( parent)
{
    m_viewName = "view1";
    m_qimage = QImage( 100, 100, QImage::Format_RGB888);
    m_qimage.fill( QColor( "blue"));
//    m_timerId = startTimer( 250);

    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;

    GetStateManager().ViewManager().RegisterView( m_viewName.toStdString(), this);
    GetStateManager().ViewManager().SetViewImageFormat( m_viewName.toStdString(), viewImageFormat);
}

MyView::~MyView()
{
    GetStateManager().ViewManager().UnregisterView(m_viewName.toStdString());
}

void MyView::SetClientSize(CSI::PureWeb::Size clientSize)
{
    std::cerr << "SetClientSize " << clientSize.Width << "x" << clientSize.Height
              << "\n";
}

CSI::PureWeb::Size MyView::GetActualSize()
{
    return CSI::PureWeb::Size(m_qimage.width(), m_qimage.height());
}

void MyView::RenderView(CSI::PureWeb::Server::RenderTarget target)
{
    CSI::ByteArray bits = target.RenderTargetImage().ImageBytes();

    if( m_qimage.format() != QImage::Format_RGB888) {
        QImage tmpImage = m_qimage.convertToFormat( QImage::Format_RGB888);
        CSI::ByteArray::Copy(tmpImage.scanLine(0), bits, 0, bits.Count());
    }
    else {
        CSI::ByteArray::Copy(m_qimage.scanLine(0), bits, 0, bits.Count());
    }

}

void MyView::PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs & ev)
{
    return;
    std::cerr << "PostKeyEvent\n"
              << "   CharacterCode: " <<  ev.CharacterCode << "\n"
              << "   EventType:" <<  ev.EventType << "\n"
              << "   KeyCode:" <<  ev.KeyCode << "\n"
              << "   Modifiers:" <<  ev.Modifiers << "\n";
}

void MyView::PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs & ev)
{
    std::cerr << "Mouse\n"
              << "   Buttons: " <<  ev.Buttons << "\n"
              << "   ChangedButton: " <<  ev.ChangedButton << "\n"
              << "   Delta: " <<  ev.Delta << "\n"
              << "   EventType:" <<  ev.EventType << "\n"
              << "   Modifiers:" <<  ev.Modifiers << "\n"
              << "   xy:" <<  ev.X << " " << ev.Y << "\n";
    m_lastMouse = QPointF( ev.X, ev.Y);


    QPointF center = m_qimage.rect().center();
    QPointF diff = m_lastMouse - center;
    double angle = atan2( diff.x(), diff.y());
    angle *= - 180 / M_PI;

    m_qimage.fill( QColor( "blue"));
    QPainter p( & m_qimage);
    p.translate( m_qimage.rect().center());
    p.rotate( angle);
    p.translate( - m_qimage.rect().center());
    p.setFont( QFont( "Arial", 20));
    p.setPen( QColor( "white"));
    p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);

    GetStateManager().ViewManager().RenderViewDeferred( m_viewName.toStdString());

}

CSI::PureWeb::Server::StateManager &MyView::GetStateManager()
{
    return *CSI::PureWeb::Server::StateManager::Instance();
}


void MyView::timerEvent(QTimerEvent *)
{
    QPointF center = m_qimage.rect().center();
    QPointF diff = m_lastMouse - center;
    double angle = atan2( diff.x(), diff.y());
    angle *= - 180 / M_PI;

    m_qimage.fill( QColor( "blue"));
    QPainter p( & m_qimage);
    p.translate( m_qimage.rect().center());
    p.rotate( angle);
    p.translate( - m_qimage.rect().center());
    p.setFont( QFont( "Arial", 20));
    p.setPen( QColor( "white"));
    p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);

    GetStateManager().ViewManager().RenderViewDeferred( m_viewName.toStdString());
}
