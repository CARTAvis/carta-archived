/**
 * Stores histogram data and associated information needed to display a histogram.
 */
#pragma once
#include <QString>
#include <vector>
#include "Plot2DResult.h"

#include <QDataStream>

namespace Carta{
namespace Lib{

namespace Hooks {

class HistogramResult : public Plot2DResult {
  

  public:
  	HistogramResult( const QString name="", const QString unitsX="", const QString unitsY="",
  		std::vector<std::pair<double,double>> data = std::vector<std::pair<double,double>>());

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
     * Sets the frequency range for a channels in the cube.
     * @param minFreq the smallest frequency for the channel range.
     * @param maxFreq the largest frequency for the channel range.
     */
    void setFrequencyBounds( double minFreq, double maxFreq );

    virtual ~HistogramResult(){}



  private:
      double m_frequencyMin;
      double m_frequencyMax;
};

//Serialization so that the histogram result can be generated in a separate process.
QDataStream &operator<<(QDataStream& out, const Carta::Lib::Hooks::HistogramResult& result );
QDataStream &operator>>(QDataStream& in, Carta::Lib::Hooks::HistogramResult& result );

}
}
}
