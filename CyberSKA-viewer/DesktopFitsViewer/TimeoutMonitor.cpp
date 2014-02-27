#include <exception>

#include <QSettings>
#include <QDir>

#include "TimeoutMonitor.h"

TimeoutMonitor::TimeoutMonitor(QObject *parent) :
    QObject(parent),
    pingCounter_(0)
{
}

void TimeoutMonitor::start ()
{
    QString settingsFilePath = QDir::homePath () + "/.sfviewer.config";
    if( ! QFileInfo( settingsFilePath).isReadable())
        throw QString("Cannot read config file %1").arg(settingsFilePath);

    // get database information from the INI file
    QSettings qset( settingsFilePath, QSettings::IniFormat);
    bool ok;
    pureWebTimeout_ = qset.value ( "timeout", DefaultPureWebTimeout).toInt ( & ok);
    if( ! ok) pureWebTimeout_ = DefaultPureWebTimeout;

    // the last timeout reset is set to current time
    lastTimeoutReset_.start ();

    dbg(1) << "Application timeout will be " << pureWebTimeout_ << " seconds.\n";

    // connect this class to pure web
    GetStateManager().ViewManager().RegisterView( "FakeView", this);
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;
    GetStateManager().ViewManager().SetViewImageFormat( "FakeView", viewImageFormat);

    startTimer ( 1000);
}

void TimeoutMonitor::PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs & /*args*/)
{
    lastTimeoutReset_.restart ();
}

void TimeoutMonitor::PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs & /*mouseEvent*/)
{
    lastTimeoutReset_.restart ();
}

void
TimeoutMonitor::timerEvent (QTimerEvent *)
{
    double remaining = pureWebTimeout_ - lastTimeoutReset_.elapsed ()/1000.0;
    if( remaining < 0.0) remaining = 0.0;

    QString val = QString("%1 %2 %3").arg( remaining, 0, 'f', 1).arg( WarningTime).arg( pingCounter_);
    pingCounter_ ++;
//    GetStateManager ().XmlStateManager ().SetValueAs<std::string>( "TimeToLive", val.toStdString ());
    pwset( "TimeToLive", val);

    // dbg(1) << "Estimated remaining time: " << remaining << " seconds.\n";
}
