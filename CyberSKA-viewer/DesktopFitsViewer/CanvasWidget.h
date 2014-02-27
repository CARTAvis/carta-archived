#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = 0);
    QImage image() { return canvasImage_; }

signals:
    void mousePress ( QMouseEvent *);
    void mouseMove ( QMouseEvent *);
    void mouseRelease (QMouseEvent *);
    void resized ( QSize);
    void mouseWheel (QWheelEvent *);
    void keyPress (QKeyEvent *);

public slots:
    void setCanvas( const QImage & image);

protected:
    // reimplemented events
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void paintEvent (QPaintEvent *event);
    virtual void resizeEvent (QResizeEvent *event);
    virtual void wheelEvent (QWheelEvent *);
    virtual void keyPressEvent (QKeyEvent *);

private:

    // this is what the canvas actually paints
    QImage canvasImage_;
};

#endif // CANVASWIDGET_H
