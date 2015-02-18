#pragma once
#include <QString>

namespace Carta{
namespace Lib{
<<<<<<< HEAD
  namespace Hooks {

=======
namespace Hooks {
>>>>>>> dd4500aa8fa1392af59a7ccfd13a56ca2591d5c1
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
}
