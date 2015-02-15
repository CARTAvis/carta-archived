#pragma once

#include "CartaLib/Hooks/HistogramResult.h"


#include <QString>

class QwtPlot;
class QwtPlotHistogram;
class QImage;

class HistogramGenerator{	
public:
  /**
   * Constructor.
   */
  HistogramGenerator();

  /**
   * Sets the data for the histogram.
   * @param data the histogram (intensity,count) pairs and additional information for plotting.
   */
  void setData( Carta::Lib::Hooks::HistogramResult data);

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
      QwtPlot *m_plot;
      QwtPlotHistogram *m_histogram;
      int m_height;
      int m_width;
};

