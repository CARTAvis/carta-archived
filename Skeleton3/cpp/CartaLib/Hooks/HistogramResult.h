/**
 * Stores histogram data and associated information needed to display a histogram.
 */
#pragma once
#include <QString>

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
  	 * Returns a user-friendly name for the data displayed in the histogram.
  	 * @return a user friendly name for the histogram data.
  	 */
    QString getName() const;
    /**
     * Returns the intensity (x-units) of the data.
     * @return the intensity units.
     */
    QString getUnits() const;
    ~HistogramResult(){}

  private:
      QString m_name;
      QString m_units;
      std::vector<std::pair<double,double>> m_data;
};
}
}
}
