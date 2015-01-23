#pragma once
#include <qwt_plot.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>

class HistogramGenerator{	
public:
  typedef std::vector<std::pair<double,double> > ResultType;
  HistogramGenerator(QString title);
  void setData( ResultType data);
  void setStyle( QString style );
  // void setColored( bool colored );
  QImage * toImage();
private:
	QwtPlot *m_plot;
	QwtPlotHistogram *m_histogram;
};

