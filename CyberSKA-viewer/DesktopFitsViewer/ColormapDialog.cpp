#include <QImage>
#include <QPainter>
#include <QLinearGradient>

#include "ColormapDialog.h"
#include "ui_ColormapDialog.h"
#include "FitsViewerLib/common.h"
#include "CanvasWidget.h"

using namespace CSI;

ColormapDialog::ColormapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColormapDialog)
{
    ui->setupUi(this);

    m_scale1 = 0;
    m_scale2 = 0;

    // connect signals/slots
    connect ( ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
              this, SLOT(listWidgetCB(QListWidgetItem*)));
    connect ( ui->reverseCheckbox, SIGNAL(toggled(bool)), this, SLOT(reportUpdate()));
    connect ( ui->invertCheckbox, SIGNAL(toggled(bool)), this, SLOT(reportUpdate()));
    connect ( ui->redSlider, SIGNAL(valueChanged(int)), this, SLOT(reportUpdate()));
    connect ( ui->greenSlider, SIGNAL(valueChanged(int)), this, SLOT(reportUpdate()));
    connect ( ui->blueSlider, SIGNAL(valueChanged(int)), this, SLOT(reportUpdate()));
    connect ( ui->canvasWidget, SIGNAL(resized(QSize)), this, SLOT(updatePreview()));

    // setup the function
    functions_["Aberration"] = ColormapFunction::aberration ();
    functions_["Fire"] = ColormapFunction::fire ();
    functions_["Gray"] = ColormapFunction::gray ();
    functions_["Heat"] = ColormapFunction::heat ();
    functions_["Mutant"] = ColormapFunction::mutant ();
    functions_["RGB"] = ColormapFunction::rgb ();
    functions_["Sea"] = ColormapFunction::sea ();
    functions_["Spring"] = ColormapFunction::spring ();
    functions_["Sunbow"] = ColormapFunction::sunbow ();
    functions_["Velocity"] = ColormapFunction::velocity ();
    functions_["CubeHelix1"] = ColormapFunction::cubeHelix( 0.5, -1.5, 1.5, 1.0);
    functions_["CubeHelix2"] = ColormapFunction::cubeHelix( 0.5, -1.5, 1.0, 0.8);

    cmap_ = functions_["Heat"]; // double check with the UI !!! TODO: better sync would be nice
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Preset", CSI::Bind(this, &ColormapDialog::OnPresetChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Invert", CSI::Bind(this, &ColormapDialog::OnInvertChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Reverse", CSI::Bind(this, &ColormapDialog::OnReverseChanged));

    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Red", CSI::Bind(this, &ColormapDialog::OnRGBChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Green", CSI::Bind(this, &ColormapDialog::OnRGBChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Blue", CSI::Bind(this, &ColormapDialog::OnRGBChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/scale1", CSI::Bind(this, &ColormapDialog::OnRGBChanged));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/scale2", CSI::Bind(this, &ColormapDialog::OnRGBChanged));

    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;

    GetStateManager().ViewManager().RegisterView("ColormapView", this);
    GetStateManager().ViewManager().SetViewImageFormat("ColormapView", viewImageFormat);
}

// returnst the current colormap
ColormapFunction
ColormapDialog::getCurrentColormap () const
{
    return cmap_;
}

// sets the current colormap
void
ColormapDialog::setNamedColormap (const QString & colormapName)
{
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->text() == colormapName)
        {
            ui->listWidget->setCurrentItem(item);
            return;
        }
    }

    // ok, we failed, just set to gray
    ui->listWidget->setCurrentItem ( ui->listWidget->item(0));
}

void
ColormapDialog::OnPresetChanged(const CSI::ValueChangedEventArgs& args)
{
    QString preset = QString(args.NewValue().ValueOr("").ToAscii().begin());
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->text() == preset)
        {
            ui->listWidget->setCurrentItem(item);
        }
    }
}


void
ColormapDialog::OnRGBChanged(const CSI::ValueChangedEventArgs & /*args*/)
{
    int red = GetStateManager().XmlStateManager().GetValueAs("/Colormap/Red", 100);
    int green = GetStateManager().XmlStateManager().GetValueAs("/Colormap/Green", 100);
    int blue = GetStateManager().XmlStateManager().GetValueAs("/Colormap/Blue", 100);
    m_scale1 = GetStateManager().XmlStateManager().GetValueAs("/Colormap/scale1", 0.0);
    m_scale2 = GetStateManager().XmlStateManager().GetValueAs("/Colormap/scale2", 0.0);

    ui->redSlider->setValue(red);
    ui->greenSlider->setValue(green);
    ui->blueSlider->setValue(blue);

    reportUpdate();
}

void
ColormapDialog::OnInvertChanged(const CSI::ValueChangedEventArgs& args)
{
    ui->invertCheckbox->setChecked(args.NewValue().ConvertOr(false));
}

void
ColormapDialog::OnReverseChanged(const CSI::ValueChangedEventArgs& args)
{
    ui->reverseCheckbox->setChecked(args.NewValue().ConvertOr(false));
}

void
ColormapDialog::listWidgetCB (
    QListWidgetItem * /*item*/)
{
    reportUpdate (); return;

//    dbg(0) << "Current colormap: " << item->text () << "\n";
//    if( functions_.contains (item->text ())){
//        cmap_ = functions_[ item->text ()];
//        updatePreview ();
//        emit colormapChanged ( cmap_ );
//    } else {
//        dbg(0) << ConsoleColors::error ()
//               << "ColormapDialog tried to set undefined color " << item->text ()
//               << ConsoleColors::resetln ();
//    }
}

void
ColormapDialog::reportUpdate ()
{
    QListWidgetItem * item = ui->listWidget->currentItem ();
    if( item == 0) return;
    if( ! functions_.contains (item->text ())) return;
    cmap_ = functions_[ item->text ()];
    cmap_.setInvert ( ui->invertCheckbox->isChecked ());
    cmap_.setReverse ( ui->reverseCheckbox->isChecked ());
    cmap_.setRgbScales (
                ui->redSlider->value () / double( ui->redSlider->maximum ()),
                ui->greenSlider->value () / double( ui->greenSlider->maximum ()),
                ui->blueSlider->value () / double( ui->blueSlider->maximum ())
                );
    cmap_.setScales( m_scale1, m_scale2);
    updatePreview ();
    emit colormapChanged ( cmap_ );

    // Update AppState

    {
        CSI::TypelessStateLock autoLock = GetStateManager().XmlStateManager().AcquireLock();
        CSI::Typeless xcolormap = autoLock.Element("Colormap");
        xcolormap["Preset"] = item->text ().toStdString();
        xcolormap["Invert"] = ui->invertCheckbox->isChecked();
        xcolormap["Reverse"] = ui->reverseCheckbox->isChecked();
        xcolormap["Red"] = ui->redSlider->value () * 100  / ui->redSlider->maximum ();
        xcolormap["Green"] = ui->greenSlider->value () * 100  / ui->greenSlider->maximum ();
        xcolormap["Blue"] = ui->blueSlider->value () * 100  / ui->blueSlider->maximum ();
    }
    pwset( "/Colormap/gamma", QString("%1").arg(cmap_.getGamma(), 0, 'g', 3));

}

ColormapDialog::~ColormapDialog()
{
    delete ui;
    GetStateManager().ViewManager().UnregisterView("ColormapView");
}

// TODO: review this code, as it was written on 2 hrs of sleep...
void
ColormapDialog::updatePreview ()
{
//    QImage img( ui->canvasWidget->size (), QImage::Format_RGB888);
    QImage img( ui->canvasWidget->size (), QImage::Format_ARGB32_Premultiplied);
    QPainter painter( & img);
    painter.setRenderHint( QPainter::Antialiasing, true);
    painter.setRenderHint( QPainter::HighQualityAntialiasing, true);

    painter.fillRect ( img.rect (), QColor("#444"));

//    int topH = std::min( 40, img.height() / 10);
//    int topH = clamp<int>( img.height() / 2, 20, 40);
    int topH = 20;
    int space = 5;

    // draw the top
    {
        QLinearGradient lg( 0, 0, img.width (), 0);
        int n = std::max( img.width (), 1000);
        for( int i = 0 ; i <= n ; i ++ ) {
            double x = double(i) / n;
            Rgb rgb = cmap_(x);
            lg.setColorAt (x, QColor( rgb.r, rgb.g, rgb.b));
        }
        painter.fillRect ( 0, 0, img.width (), topH, lg);
    }

    // draw the bottom
    {
        // TODO: instead of sampling at arbitrary number of points,
        //       use the PWLinear control points to figure out where to
        //       sample... duh!!! much faster, less jaggy lines
        int n = std::max( img.width (), 1000);
        QVector<Rgbf> cache; cache.resize (n);
        for( int i = 0 ; i < n ; i ++ ) {
            double x = double(i) / double(n-1);
            Rgb col = cmap_(x);
            cache[i] =  Rgbf( col.r / 255.0, col.g / 255.0, col.b / 255.0);
        }
        double top = topH + space;
        double bottom = img.height () - space;
        double penWidth = 2;

        QVector<QPointF> redPts, greenPts, bluePts;
        double x, y;
        for( int i = 0 ; i < cache.size () ; i ++ ) {
            x = double(i) / n;
            x *= img.width ();
            y = cache[i].r ;
            y = (1-y) * bottom + y * top;
            redPts << QPointF(x,y);
            y = cache[i].g ;
            y = (1-y) * bottom + y * top;
            greenPts << QPointF(x,y);
            y = cache[i].b ;
            y = (1-y) * bottom + y * top;
            bluePts << QPointF(x,y);
        }
        painter.save();
        painter.setCompositionMode( QPainter::CompositionMode_Plus);
        painter.setPen ( QPen( QColor( 255, 0, 0), penWidth));
        painter.drawPolyline( & redPts[0], redPts.size());
        painter.setPen ( QPen( QColor( 0, 255, 0), penWidth));
        painter.drawPolyline( & greenPts[0], greenPts.size());
        painter.setPen ( QPen( QColor( 0, 0, 255), penWidth));
        painter.drawPolyline( & bluePts[0], bluePts.size());
        painter.restore();
    }

    GetStateManager().ViewManager().RenderViewDeferred("ColormapView");
    ui->canvasWidget->setCanvas (img);

    buffer_ = img.convertToFormat( QImage::Format_RGB888);
}

// PureWeb: IRenderedView
void ColormapDialog::SetClientSize(CSI::PureWeb::Size clientSize)
{
    ui->canvasWidget->resize( clientSize.Width, clientSize.Height);
}

CSI::PureWeb::Size ColormapDialog::GetActualSize()
{
    return CSI::PureWeb::Size(buffer_.width(), buffer_.height());
}

void ColormapDialog::RenderView(CSI::PureWeb::Server::RenderTarget image)
{
    ByteArray bits = image.RenderTargetImage().ImageBytes();
    ByteArray::Copy(buffer_.scanLine(0), bits, 0, bits.Count());
}

void ColormapDialog::PostMouseEvent(
        const CSI::PureWeb::Ui::PureWebMouseEventArgs & /*mouseEvent*/)
{
}


