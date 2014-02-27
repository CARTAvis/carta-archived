#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QSharedPointer>
#include <QTimer>

#include "FitsViewerLib/CachedImageReader.h"
#include "FitsViewerLib/FitsParser.h"
#include "FitsViewerLib/FitsViewerServer.h"
#include "GlobalState.h"

#include "PureWeb.h"

class HistogramDialog : public QObject, CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent, FitsViewerServer & fvs);
    ~HistogramDialog();

    void setFrameInfo(
            qint64 frameNo,
            const QSharedPointer < CachedImageReader > m_cir);

    double min() const { return min_; }
    double max() const { return max_; }

signals:
    // emmited whenver the user adjusts histogram values, either by presets
    // or by sliders
    void valuesChanged( double min, double max);

public slots:
    void activatePreset(int id, bool zoom = false);
    void zoomToSelection();
    void showFullRange();

protected slots:
    void recalcTimerCB();

protected:

    // PureWeb - IRenderedView pure virtual methods
    void SetClientSize(CSI::PureWeb::Size clientSize);
    CSI::PureWeb::Size GetActualSize();
    void RenderView(CSI::PureWeb::Server::RenderTarget image);
    void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs& /*args*/) {}
    void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& /*mouseEvent*/) {}

    QTimer m_recalcTimer;

    void recalculateHistogramDelayed();
    void recalculateHistogramRaw();
    void pwCommandHandler(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);

    RaiLib::HistogramInfo frameInfo_;
    qint64 m_frameInfoNumber; // frame number of the frameInfo_

    double gMin_, gMax_; // global min/max (same as data in frameInfo_)
    double zMin_, zMax_; // current (zoom) min/max
    double min_, max_; // current 'slider' positions
    QColor bgColor_, markerColor_, rectColor_;

    // pointer (reference) to fits viewer server
    FitsViewerServer & m_fvs;

    // info about histogram
    struct {
        QVector<quint64> bins;
        double min, max;
        quint64 maxBin;
        quint64 nNans;
    } m_histInfo;
    int m_imageWidth, m_imageHeight;
    bool m_smoothGraph, m_logScale;
    int m_cursorX;
    int m_marginLeft, m_marginRight, m_marginTop, m_marginBottom;
    bool m_visibleOnClient;

    QSharedPointer < CachedImageReader > m_cir;

    // image for rendering the histogram
    QImage m_buffer;

    // shared variables
    struct {
        GlobalState::DoubleVar::UniquePtr cursor;
        GlobalState::DoubleVar::UniquePtr lowerClip;
        GlobalState::DoubleVar::UniquePtr upperClip;
    } m_vars;

    // guard the callbacks from global variables until everything is constructed
    bool m_ignoreVarCallbacks /*= true*/;

    // global variable callback (generic)
    void globalVariableCB( const QString & name);

    // helper function to help us create bindings for global variables
    template <class T>
    void binder(
            std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
            const QString & prefix,
            const QString & name)
    {
        ptr.reset( new GlobalState::TypedVariable<T>(prefix + name));
        std::function<void()> cb = std::bind(&HistogramDialog::globalVariableCB, this, name);
        ptr->addCallback( cb);
    }


};

#endif // HISTOGRAMDIALOG_H
