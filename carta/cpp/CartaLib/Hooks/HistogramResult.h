/**
 * Stores histogram data and associated information needed to display a histogram.
 */
#pragma once
#include <QString>
#include <vector>

namespace Carta{
namespace Lib{

namespace Hooks {

class HistogramResult {
  

  public:
  	HistogramResult( const QString name="", const QString units="",
  		std::vector<std::pair<double,double>> data = std::vector<std::pair<double,double>>());

  	/**
     * Returns the (intensity,count) pairs representing the histogram data.
     * @return a vector containing (intensity,count) pairs.
     */
    std::vector<std::pair<double,double>> getData() const;

    /**
     * Returns the minimum frequency for a range selection.
     * @return the minimum frequency for a cube channel range selection.
     */
    double getFrequencyMin() const;

    /**
     * Returns the maximum frequency for a range selection.
     * @return the maximum frequency for a cube channel range selection.
     */
    double getFrequencyMax() const;

  	/**
  	 * Returns a user-friendly name for the data displayed in the histogram.
  	 * @return a user friendly name for the histogram data.
  	 */
    QString getName() const;

    /**
     * Returns the intensity (x-units) of the data.
     * @return the intensity units.
     */
    QString getUnits() const;

    /**
     * Sets the frequency range for a channels in the cube.
     * @param minFreq the smallest frequency for the channel range.
     * @param maxFreq the largest frequency for the channel range.
     */
    void setFrequencyBounds( double minFreq, double maxFreq );

    ~HistogramResult(){}

  private:
      QString m_name;
      QString m_units;
      double m_frequencyMin;
      double m_frequencyMax;
      std::vector<std::pair<double,double>> m_data;
};
}
}
}
