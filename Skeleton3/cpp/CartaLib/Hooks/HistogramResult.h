#pragma once
#include <QString>

namespace Carta{
namespace Lib{

class HistogramResult {
  private:
  	QString name;
  	std::vector<std::pair<double,double>> data;
  
  public:
  	HistogramResult( QString name = "", 
  		std::vector<std::pair<double,double>> data = std::vector<std::pair<double,double>>());
    QString getName() const;
    ~HistogramResult(){}
    std::vector<std::pair<double,double>> getData() const;
};
}
}
