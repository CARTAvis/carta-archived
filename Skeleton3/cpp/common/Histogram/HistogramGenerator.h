/**
 * Generates an image of a histogram based on set configuration (display) parameters.
 */
#pragma once

#include "CartaLib/Hooks/HistogramResult.h"
#include <QFont>
#include <QString>
#include <memory>

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

class QwtPlot;
class QwtPlotPicker;
class QImage;

namespace Carta {
namespace Histogram {

class HistogramPlot;
class HistogramSelection;

class HistogramGenerator{

public:
  /**
   * Constructor.
   */
  HistogramGenerator();

  /**
   * Gets the m_minClip and m_maxClip.
   */
  std::vector<double> getHistogramClips();

  /**
   * Gets new clips calculated in histogram selection and updates them on the plot
   */
  void updateHistogramClips();
  /**
   * Sets the data for the histogram.
   * @param data the histogram (intensity,count) pairs and additional information for plotting.
   */
  void setData( Carta::Lib::Hooks::HistogramResult data, double minIntensity, double maxIntensity);

  /**
   * Set the size of the image that will be generated.
   * @param width the width of the generated image.
   * @param height the height of the generated image.
   */
  void setSize( int width, int height );

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
  void setHistogramSelection(double min, double max);
  
  /**
   * Set whether or not the user is selecting a range on the histogram.
   * @param selecting true if a range is currently being selected; false otherwsie.
   */
  void setSelectionMode(bool selecting);

  /**
   * Set whether or not the histogram should be colored based on intensity values.
   * @param colored true if the histogram should be colored; false if it should be drawn in just a single color.
   */
  void setColored( bool colored );

  /**
   * Set the name of the color map that can be used as a look-up for mapping intensity to color.
   * @param cMap the object which maps intensity to color.
   */
  void setColorMap( std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> cMap );

  /**
   * Returns the QImage reflection the current state of the histogram.
   * @return QImage the histogram image.
   */
  QImage * toImage();
private:
  const static double EXTRA_RANGE_PERCENT;
  QwtPlot *m_plot;
  HistogramPlot* m_histogram;
  HistogramSelection *m_range;
  QwtPlotPicker* m_dragLine;
  int m_height;
  int m_width;
  QFont m_font;
  double m_clipMin;
  double m_clipMax;
  std::vector<double> _getAxisRange(double minIntensity, double maxIntensity);
  // void updateHistogramRange( double minClip, double maxClip );
};
}
}
