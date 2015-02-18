#pragma once
#include <qwt_plot.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>
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

class HistogramGenerator : public QWidget{
  Q_OBJECT

friend class HistogramSelection;	
public:
  typedef std::vector<std::pair<double,double> > ResultType;
  HistogramGenerator();
  void setData( Carta::Lib::Hooks::HistogramResult data, double minIntensity, double maxIntensity);
  void setStyle( QString style );
  void setLogScale(bool display);
  void setHistogramRange(double min, double max);
  // void setColored( bool colored );
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

