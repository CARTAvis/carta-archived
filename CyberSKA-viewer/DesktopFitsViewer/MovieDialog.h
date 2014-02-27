#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTime>

#include "PureWeb.h"

namespace Ui {
    class MovieDialog;
};

class IntegerControl;

class MovieDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieDialog (QWidget * parent = 0);
    ~MovieDialog();

public slots:
    void setNFrames( int nFrames);
    void startMovie();
    void stopMovie();
    void frameChanged( int frameNo);
    void mainViewIsSynchronizedCB();

signals:
    void frameRequested (int frameNo);

protected slots:
    void requestFrame( int frameNo);

protected:
    Ui::MovieDialog * m_ui;
    int m_nFrames;
    int m_currentFrame;
    int m_startFrame;
    int m_endFrame;
    int m_frameSkip;
    int m_frameDelay;
    bool m_isBouncing;
    enum { Play, Reverse, Stop } m_status;
    IntegerControl * currentFrameControl_ ;
    IntegerControl * delayControl_;
    int timerId_;
    QTime m_frameRequestTime;

    virtual void timerEvent (QTimerEvent *);
    virtual void closeEvent (QCloseEvent *);

    void sendStateToClients();
    void playNextFrame();

    /// client callbacks
    void clientFrameRequestCB( const CSI::ValueChangedEventArgs& args);
    void clientCommandCB(CSI::Guid sessionId, CSI::Typeless command, CSI::Typeless responses);
};

class CanvasSliderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasSliderWidget (QWidget * parent = 0);

signals:
    void valueChanged( int);

protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void mousePressEvent (QMouseEvent *);

// private:
    int min_, max_, val_;

    friend class MovieDialog; // TODO: don't be lazy

};

// helper class to manage the integer controls
// TODO: a much better approach would be create a proper widget
//       but extra would would be needed to make it usable in a qcreator (i.e. plugin)
class IntegerControl : public QObject
{
    Q_OBJECT

public:
    IntegerControl(
        QObject * parent,
        QPushButton * minus10button,
        QPushButton * minus1button,
        QPushButton * plus1button,
        QPushButton * plus10button,
        QLabel * valueLabel
        );
    int getValue() const;
    void setValue( int value, bool callback = false);
    void setMinMax( int min, int max);

signals:
    void valueChanged( int value);

protected slots:
    void minus10CB();
    void minus1CB();
    void plus1CB();
    void plus10CB();

private:

    QPushButton * minus10button_, * minus1button_, * plus1button_, * plus10button_;
    QLabel * valueLabel_;
    int min_, max_, value_;
};

//class Kvis

#endif // MOVIEDIALOG_H
