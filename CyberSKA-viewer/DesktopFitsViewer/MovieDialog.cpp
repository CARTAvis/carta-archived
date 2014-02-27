// TODO: This class is a total mess
// 1.) the GUI part should be completely removed
// 2.) the class should be renamed to something like 'movie controller'


#include "MovieDialog.h"

#include <cmath>
#include <iostream>

#include <QPainter>
#include <QMouseEvent>
#include <QTime>

#include "ui_MovieDialog.h"
#include "FitsViewerLib/common.h"

#include "PureWeb.h"

MovieDialog::MovieDialog(QWidget *parent) :
    QDialog(parent)

{
    m_ui = new Ui::MovieDialog;

    m_status = Stop;
    m_ui->setupUi(this);
    m_ui-> csWidget-> min_ = 0;

    currentFrameControl_ = new IntegerControl(
                this,
                m_ui-> currentFrameMinus10button,
                m_ui-> currentFrameMinus1button,
                m_ui-> currentFramePlus1button,
                m_ui-> currentFramePlus10button,
                m_ui-> currentFrameLabel );
    delayControl_ = new IntegerControl(
                this,
                m_ui-> intervalMinus10button,
                m_ui-> intervalMinus1button,
                m_ui-> intervalPlus1button,
                m_ui-> intervalPlus10button,
                m_ui-> intervalLabel );
    delayControl_-> setMinMax ( 0, 99999);
    delayControl_-> setValue (200);
    connect ( currentFrameControl_, SIGNAL(valueChanged(int)), this, SLOT(requestFrame(int)));
    connect ( m_ui-> csWidget, SIGNAL(valueChanged(int)),
              this, SLOT(requestFrame(int)));
    connect ( m_ui-> startButton, SIGNAL(clicked()), this, SLOT(startMovie()));
    connect ( m_ui-> stopButton, SIGNAL(clicked()), this, SLOT(stopMovie()));
    m_frameDelay = 200;
    m_isBouncing = false;
    setNFrames ( 10);
    requestFrame ( 0);

    timerId_ = 0;

    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/movie/frame",
                CSI::Bind(this, &MovieDialog::clientFrameRequestCB));
    GetStateManager().CommandManager().AddUiHandler(
                "/movie/command", CSI::Bind(this, &MovieDialog::clientCommandCB));

    sendStateToClients();

}

MovieDialog::~MovieDialog()
{
    delete m_ui;
    delete currentFrameControl_;
}

void
MovieDialog::clientFrameRequestCB (const CSI::ValueChangedEventArgs &args)
{
    QString dcString = args.NewValue().ConvertOr<QString>("1 1");
    QStringList vals = dcString.split( ' ');
    if( vals.length() < 1) {
        dbg(1) << ConsoleColors::error () << "Bad frame request '"
               << dcString << "' "
               << ConsoleColors::reset () << "\n";
        return;
    }
    bool ok;
    int frame = vals[0].toInt( & ok);
    if( ! ok) {
        dbg(1) << ConsoleColors::error () << "Bad frame request '"
               << dcString << "' "
               << ConsoleColors::reset () << "\n";
        return;
    }
    frame --; // clients start counting at 1

    dbg(1) << "client requested frame " << frame;

//    int frame = args.NewValue().ConvertOr(1) - 1;
    frame = clamp( frame, 0, m_nFrames - 1);
    if (frame != m_currentFrame)
        requestFrame(frame);
}

void MovieDialog::clientCommandCB(CSI::Guid /*sessionId*/, CSI::Typeless command, CSI::Typeless /*responses*/)
{
    QString cmd = command["/cmd"].ValueOr<QString>("");
    QString val = command["/val"].ValueOr<QString>("");
    dbg(1) << "Movie command: " << cmd << " " << val << "\n";

    if( cmd == "stop") {
        stopMovie();
    }
    else if( cmd == "play") {
        m_status = Play;
//        this->stopMovie();
//        this->startMovie();
//        sendStateToClients();
        playNextFrame();
    }
    else if( cmd == "rplay") {
        m_status = Reverse;
//        this->stopMovie();
//        this->startMovie();
//        sendStateToClients();
        playNextFrame();
    }
    else if( cmd == "setFrameDelay") {
        m_frameDelay = command["/val"].ValueOr<int>(200);
        delayControl_->setValue(m_frameDelay);
        sendStateToClients();
    }
    else if( cmd == "setStartFrame") {
        m_startFrame = command["/val"].ValueOr<int>(1) - 1;
        sendStateToClients();
    }
    else if( cmd == "setEndFrame") {
        m_endFrame = command["/val"].ValueOr<int>( m_nFrames) - 1;
        sendStateToClients();
    }
    else if( cmd == "setFrameSkip") {
        m_frameSkip = command["/val"].ValueOr<int>(1);
        sendStateToClients();
    }
    else if( cmd == "setBouncing") {
        m_isBouncing = ( val == "true");
        sendStateToClients();
    }

}

void
MovieDialog::requestFrame (
    int frameNo)
{
    // let anyone interested know we have requested a frame
    emit frameRequested ( frameNo);

    // remember the time when we made the frame request
    m_frameRequestTime.restart();
}

void MovieDialog::frameChanged(int frameNo)
{
    m_currentFrame = frameNo;
    sendStateToClients();
}

void MovieDialog::mainViewIsSynchronizedCB()
{
    dbg(1) << "frame delay = " << m_frameRequestTime.elapsed() << "\n";

    // if the movie is not in play mode, do nothing at all
    if( m_status == Stop) return;

    // otherwise figure out how much time we still need to satisfy the delay
    // and request the next frame in that interval
    int diff = m_frameDelay - m_frameRequestTime.elapsed();
    if( diff < 0) diff = 0;

    if( timerId_ != 0) {
        killTimer( timerId_);
    }
    dbg(1) << "frame delay diff " << diff  << "\n";
    timerId_ = startTimer( diff);

//    playNextFrame();
}

void
MovieDialog::setNFrames (
    int nFrames)
{
    m_nFrames = nFrames;
    m_currentFrame = 0;
    m_startFrame = 0;
    m_endFrame = nFrames - 1;
    m_frameSkip = 1;

    m_ui-> csWidget-> max_ = m_nFrames -1;
    m_ui-> csWidget-> val_ = m_currentFrame;
    m_ui-> csWidget-> update ();
    currentFrameControl_-> setMinMax ( 0, nFrames - 1);

    sendStateToClients();
}

void
MovieDialog::startMovie ()
{
    m_ui-> startButton->setChecked ( true);
    m_ui-> stopButton->setChecked ( false);

    if( timerId_ != 0) {
        killTimer( timerId_);
    }
    timerId_ = startTimer ( m_frameDelay);

    playNextFrame();
}

void
MovieDialog::stopMovie ()
{
    m_ui-> startButton->setChecked ( false);
    m_ui-> stopButton->setChecked ( true);
    if( timerId_ != 0) {
        killTimer ( timerId_);
        timerId_ = 0;
    }
    m_status = Stop;
    sendStateToClients();
}

void
MovieDialog::timerEvent (
    QTimerEvent *)
{
    if( timerId_ != 0) {
        killTimer ( timerId_);
        timerId_ = 0;
    }

    playNextFrame();
}

// trap dialog close so we can stop the movie
void
MovieDialog::closeEvent (
    QCloseEvent * event)
{
    stopMovie ();
    event-> accept ();
}

/**
 * @brief MovieDialog::sendStateToClients
 *
 * Send a state to clients. The client counts frames from 1, but we count from 0. Be warned.
 */
void MovieDialog::sendStateToClients()
{
    QString status;
    if( m_status == Stop) {
        status = "stop";
    } else if( m_status == Play) {
        status = "play";
    } else {
        status = "rplay";
    }
    pwset( "/Movie/status", status);

    pwset( "/Movie/frameDelay", m_frameDelay);
    pwset( "/Movie/startFrame", m_startFrame + 1);
    pwset( "/Movie/endFrame", m_endFrame + 1);
    pwset( "/Movie/frameSkip", m_frameSkip);
    pwset( "/Movie/isBouncing", m_isBouncing ? "true" : "false");
    pwset( "/Movie/CurrentFrame", m_currentFrame + 1 );
    pwset( "/Movie/TotalFrames", m_nFrames);

    pwsetdc( "/Movie/dc", drand48());
}

void MovieDialog::playNextFrame()
{
    if( m_status == Stop) return;
    if( m_nFrames < 2) return;

    int nextFrame = m_currentFrame;
    if( m_status == Play) {
        nextFrame += m_frameSkip;
        if( nextFrame > m_endFrame) {
            if( m_isBouncing) {
                m_status = Reverse;
                nextFrame -= 2 * m_frameSkip;
            } else {
                nextFrame = m_startFrame;
            }
        }
    } else {
        nextFrame -= m_frameSkip;
        if( nextFrame < m_startFrame) {
            if( m_isBouncing) {
                m_status = Play;
                nextFrame += 2 * m_frameSkip;
            } else {
                nextFrame = m_endFrame;
            }
        }
    }
    sendStateToClients();
    requestFrame (nextFrame);
}

// constructor for the graphical slider widget
CanvasSliderWidget::CanvasSliderWidget(
    QWidget * parent
    ) :
    QWidget (parent)
{
    min_ = 0;
    max_ = 99;
    val_ = 50;
}

// paint method
void
CanvasSliderWidget::paintEvent (
    QPaintEvent *)
{
    QPainter painter ( this);
    painter.eraseRect ( this->rect ());

    int x = this-> width ()/2;

    if( min_ != max_ ) {
        double xi = double(val_) / ( max_ - min_);
        x = round (xi * (this->width ()-3) + 2);
    }

    painter.setPen ( QPen( QColor( "white"), 1.1));
    painter.drawLine ( QPointF( x, 0), QPointF( x, this->height ()));
}

// mouse listeners
void
CanvasSliderWidget::mouseMoveEvent (
    QMouseEvent * event)
{
    if( event->buttons () != Qt::LeftButton) return;

    double vi = double(event->pos ().x ()) / this->width ();
    int v = vi * (max_ - min_) + min_;
    if( v < min_) v = min_;
    if( v > max_) v = max_;
    val_ = v;
    update();
    emit valueChanged ( val_);
}
void CanvasSliderWidget::mousePressEvent (
    QMouseEvent * event)
{
    // right now press & move are the same
    mouseMoveEvent ( event);
}

// constructor for IntegerWidget
IntegerControl::IntegerControl(
    QObject *parent,
    QPushButton *minus10button,
    QPushButton *minus1button,
    QPushButton *plus1button,
    QPushButton *plus10button,
    QLabel *valueLabel
    ) :
    QObject(parent)
{
    value_ = 0;
    // remember the widgets
    minus10button_ = minus10button;
    minus1button_ = minus1button;
    plus1button_ = plus1button;
    plus10button_ = plus10button;
    valueLabel_ = valueLabel;

    // do some extra GUI stuff
    valueLabel_-> setTextFormat ( Qt::PlainText);

    // connect signals
    connect( minus10button_, SIGNAL(clicked()), this, SLOT(minus10CB()));
    connect( minus1button_, SIGNAL(clicked()), this, SLOT(minus1CB()));
    connect( plus1button_, SIGNAL(clicked()), this, SLOT(plus1CB()));
    connect( plus10button_, SIGNAL(clicked()), this, SLOT(plus10CB()));

    setMinMax ( 0, 100);
    setValue ( 50, false);
}

void
IntegerControl::minus10CB ()
{
    setValue ( getValue () - 10, true);
}

void
IntegerControl::minus1CB ()
{
    setValue ( getValue () - 1, true);
}

void
IntegerControl::plus1CB ()
{
    setValue ( getValue () + 1, true);
}

void
IntegerControl::plus10CB ()
{
    setValue ( getValue () + 10, true);
}

void
IntegerControl::setValue (
    int value,
    bool callback)
{
    if( value == value_) return;
    if( value < min_) value = min_;
    if( value > max_) value = max_;
    value_ = value;
    valueLabel_-> setText ( QString("%1").arg (value_));

    if( callback) {
        emit valueChanged (value_);
    }
}

int
IntegerControl::getValue () const
{
    return value_;
}

void
IntegerControl::setMinMax (int min, int max)
{
    min_ = min;
    max_ = max;
    setValue ( getValue ()); // clip just in case
}
