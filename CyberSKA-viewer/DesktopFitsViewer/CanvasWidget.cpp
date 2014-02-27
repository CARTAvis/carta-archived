#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>

#include "CanvasWidget.h"
#include "FitsViewerLib/common.h"

CanvasWidget::CanvasWidget(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking ( true);
}

void CanvasWidget::mousePressEvent (QMouseEvent * event) {
    emit mousePress (event);
}
void CanvasWidget::mouseMoveEvent (QMouseEvent * event) {
    emit mouseMove (event);
}
void CanvasWidget::mouseReleaseEvent (QMouseEvent * event) {
    emit mouseRelease (event);
}
void CanvasWidget::wheelEvent (QWheelEvent * event) {
    emit mouseWheel (event);
}
void CanvasWidget::keyPressEvent (QKeyEvent * event) {
    emit keyPress (event);
}

void
CanvasWidget::resizeEvent (
    QResizeEvent *event
    )
{
    QWidget::resizeEvent ( event);
    emit resized (event-> size ());
}

void
CanvasWidget::paintEvent (
    QPaintEvent * event
    )
{
    QWidget::paintEvent ( event );
    QPainter painter( this);
    if( ! canvasImage_.isNull ()) {
        painter.drawImage ( this->rect (), canvasImage_ );
    }
    else {
        dbg(0) << "Drawing default canvas\n";
        painter.setOpacity ( 0.1);
        painter.setRenderHints ( QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.fillRect ( this-> rect (), QColor( "grey"));
//        painter.setBrush ( QColor( "yellow"));
//        painter.setPen ( QPen( QColor( qRgba (0,0,255,1)), 7));
//        painter.setPen ( QPen(QColor( 0, 0, 0, 128), 7));
//        painter.drawEllipse ( QPoint(this->width ()/2, this->height ()/2),
//                              this->width ()/2, this->height ()/2);
        painter.setBrush ( QColor( "black"));
        painter.setOpacity ( 1);
        QFont font = painter.font ();
        font.setPixelSize (42);
        painter.setFont ( font);
        painter.drawText ( this->rect (), "No image given",
                           QTextOption( Qt::AlignVCenter | Qt::AlignHCenter));
    }
}


void
CanvasWidget::setCanvas (
    const QImage & image
    )
{
    canvasImage_ = image;
    update ();
}
