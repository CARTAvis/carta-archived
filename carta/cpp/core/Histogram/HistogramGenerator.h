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
   * Return the minimum and maximum value of the user's zoom selection.
   * @param valid true if there is a zoom selection with a minimum/maximum value; false otherwise.
   * @return the zoom selection range.
   */
  std::pair<double,double> getRange(bool* valid ) const;

  /**
   * Return the minimum and maximum value of the user's clip selection.
   * @param valid true if there is a clip selection with a minimum/maximum value; false otherwise.
   * @return the clip selection range.
   */
  std::pair<double,double> getRangeColor(bool* valid ) const;

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
   * Set the min and max zoom intensity range for the histogram.
   * @param min the minimum zoom intensity value.
   * @param max the maximum zoom intensity value.
   */
  void setRangePixels(double min, double max);

  /**
   * Set the min and max clip intensity range.
   * @param min the minimum clip intensity value.
   * @param max the maximum clip intensity value.
   */
  void setRangePixelsColor( double min, double max );
  
  /**
   * Set whether or not the user is selecting a zoom range on the histogram.
   * @param selecting true if a zoom range is currently being selected; false otherwsie.
   */
  void setSelectionMode(bool selecting);

  /**
   * Set whether or not the user is selection a clip range on the histogram.
   * @param selection true if a clip range is being selected; false otherwise.
   */
  void setSelectionModeColor( bool selection );

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
   * Sets the zoom selection range in world coordinates.
   * @param min the minimum zoom range in intensity units.
   * @param max the maximum zoom range in intensity units.
   */
  void setRangeIntensity(double min, double max);

  /**
   * Sets the clip selection range in world coordinates.
   * @param min the minimum clip range value in intensity units.
   * @param max the maximum clip range value in intensity units.
   */
  void setRangeIntensityColor(double min, double max);

  /**
   * Returns the QImage reflection the current state of the histogram.
   * @return QImage the histogram image.
   */
   QImage * toImage() const;

   virtual ~HistogramGenerator();

private:
  void _setVerticalAxisTitle();
  const static double EXTRA_RANGE_PERCENT;
  QwtPlot *m_plot;
  HistogramPlot* m_histogram;
  HistogramSelection *m_range;
  HistogramSelection * m_rangeColor;
  int m_height;
  int m_width;
  bool m_logCount;
  int m_maxCount;
  QFont m_font;
};
}
}
