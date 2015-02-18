#pragma once

#include "CartaLib/Hooks/HistogramResult.h"
#include "HistogramSelection.h"
#include <QWidget>
#include <QRect>
#include <QRectF>
#include <QPainter>
#include <qwt_plot_renderer.h>
#include <QImage>
#include <QPaintDevice>
#include <QString>
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <QObject>
#include "HistogramPlot.h"
#include <QString>

class QwtPlot;
class QwtPlotHistogram;
class QImage;


class HistogramGenerator : public QWidget{
  Q_OBJECT

friend class HistogramSelection;	

public:
  /**
   * Constructor.
   */
  HistogramGenerator();

  /**
   * Sets the data for the histogram.
   * @param data the histogram (intensity,count) pairs and additional information for plotting.
   */
  void setData( Carta::Lib::Hooks::HistogramResult data, double minIntensity, double maxIntensity);

  /**
   * Set the drawing style for the histogram.
   * @param style {QString} the histogram draw style.
   */
  void setStyle( QString style );

  /**
   * Set whether or not the histogram should use a log scale.
   * @param logScale true if the y-axis should use a log scale; false otherwise.
   */
  void setLogScale(bool logScale);

  /**
   * Set the min and max intensity range for the histogram.
   * @param min the minimum intensity value.
   * @param max the maximum intensity value.
   */
  void setHistogramRange(double min, double max);
  // void setColored( bool colored );

  /**
   * Returns the QImage reflection the current state of the histogram.
   * @return QImage the histogram image.
   */
  QImage * toImage();
private:
  const static double EXTRA_RANGE_PERCENT;
	QwtPlot *m_plot;
	//QwtPlotHistogram *m_histogram;
  HistogramPlot *m_histogram;
  HistogramSelection *m_range;
  QwtPlotPicker* m_dragLine;
  int m_height;
  int m_width;
  std::vector<double> _getAxisRange(double minIntensity, double maxIntensity);
private slots:
  void lineMoved( const QPointF& pt );
  void lineSelected();

};

