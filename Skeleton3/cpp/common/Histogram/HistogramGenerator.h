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
            class CustomizablePixelPipeline;
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
   * Return the minimum and maximum value of the user's selection.
   * @param valid true if there is a selection with a minimum/maximum value; false otherwise.
   */
  std::pair<double,double> getRange(bool* valid ) const;

  /**
   * Gets new clips calculated in histogram selection and updates them on the plot
   */
  void updateHistogramClips();
  /**
   * Sets the data for the histogram.
   * @param data the histogram (intensity,count) pairs and additional information for plotting.
   */
  void setData( Carta::Lib::Hooks::HistogramResult data);

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
  void setRangePixels(double min, double max);
  
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
   * Set the pipeline used to map intensity to color.
   * @param pipeline the mapping from intensity to color.
   */
  void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

  /**
   * Returns the QImage reflection the current state of the histogram.
   * @return QImage the histogram image.
   */
  QImage * toImage();
private:
  void _setVerticalAxisTitle();
  const static double EXTRA_RANGE_PERCENT;
  QwtPlot *m_plot;
  HistogramPlot* m_histogram;
  HistogramSelection *m_range;
  QwtPlotPicker* m_dragLine;
  int m_height;
  int m_width;
  bool m_logCount;
  int m_maxCount;
  QFont m_font;
};
}
}
