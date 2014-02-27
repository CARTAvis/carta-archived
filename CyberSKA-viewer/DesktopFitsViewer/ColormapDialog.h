#ifndef COLORMAPDIALOG_H
#define COLORMAPDIALOG_H

#include <limits>
#include <cmath>

#include <QDialog>
#include <QListWidgetItem>
#include <QPointF>
#include <QList>
#include <QMap>

#include "FitsViewerLib/common.h"
#include "FitsViewerLib/ColormapFunction.h"

#include "PureWeb.h"

namespace Ui {
    class ColormapDialog;
}

class ColormapDialog : public QDialog, CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT

public:
    explicit ColormapDialog(QWidget *parent = 0);
    ~ColormapDialog();
    ColormapFunction getCurrentColormap(void) const;

signals:
    void colormapChanged( ColormapFunction);

public slots:
    void reportUpdate();
    void setNamedColormap( const QString & colormapName);

private slots:
    void listWidgetCB( QListWidgetItem *);
    void updatePreview();

private:
    Ui::ColormapDialog *ui;

    // PureWeb handlers
    void OnPresetChanged(const CSI::ValueChangedEventArgs& args);
    void OnInvertChanged(const CSI::ValueChangedEventArgs& args);
    void OnReverseChanged(const CSI::ValueChangedEventArgs& args);
    void OnRGBChanged(const CSI::ValueChangedEventArgs& args);

//    ColormapFunction grayFunction_;
//    ColormapFunction heatFunction_;

    ColormapFunction cmap_; // current colormap
    QMap< QString, ColormapFunction > functions_;

    // PureWeb
    // IRenderedView
    void SetClientSize(CSI::PureWeb::Size clientSize);
    CSI::PureWeb::Size GetActualSize();
    void RenderView(CSI::PureWeb::Server::RenderTarget image);
    void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs& /*args*/) {}
    void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& mouseEvent);
    QImage buffer_;

    double m_scale1, m_scale2;
};

#endif // COLORMAPDIALOG_H
