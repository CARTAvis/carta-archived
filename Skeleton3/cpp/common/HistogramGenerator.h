#pragma once
#include <qwt_plot.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>

class HistogramGenerator{	
public:
  HistogramGenerator(QString title);
  void setData( QVector<QwtIntervalSample>& samples);
  void setStyle( QwtPlotHistogram::HistogramStyle style = QwtPlotHistogram::Outline,
   QPen color = QPen(Qt::blue), QBrush fill = Qt::NoBrush);
  QImage * toImage();
private:
	QwtPlot *m_plot;
	QwtPlotHistogram *m_histogram;
};

