#include <HistogramResult.h>
namespace Carta {
  namespace Lib {
HistogramResult::HistogramResult( QString histogramName, 
	std::vector<std::pair<double,double>> histogramData ){

	name = histogramName;
	data = histogramData;

}

QString HistogramResult::getName() const{
	return name;
}

std::vector<std::pair<double,double>> HistogramResult::getData() const{
	return data;
}
  }
}
