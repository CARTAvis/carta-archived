/**
 * Stores 2D plot data and associated information needed to display it.
 */
#pragma once
#include <QString>
#include <vector>

namespace Carta{
namespace Lib{

namespace Hooks {

class Plot2DResult {
  

  public:

    /**
     * Constructor.
     * @param name - the name of the plot.
     * @param unitsX - units for the x-axis.
     * @param unitsY - units for the y-axis.
     * @param data - (x,y) pairs to be plotted.
     */
  	Plot2DResult( const QString name="", const QString unitsX="", const QString unitsY="",
  		std::vector<std::pair<double,double>> data = std::vector<std::pair<double,double>>());

  	/**
     * Returns the (x,y) pairs representing the plot data.
     * @return a vector containing (x,y) pairs.
     */
    std::vector<std::pair<double,double>> getData() const;

  	/**
  	 * Returns a user-friendly plot title.
  	 * @return a user friendly plot titlee.
  	 */
    QString getName() const;

    /**
     * Returns the units for the x-axis.
     * @return - the x-axis units.
     */
    QString getUnitsX() const;

    /**
     * Returns the units for the y-axis.
     * @return - the y-axis units.
     */
    QString getUnitsY() const;

    /**
     * Store the 2-dimensional data.
     * @param result - a list of (x,y) values.
     */
    void setData( const std::vector<std::pair<double,double> >& result );

    /**
     * Set the title of the plot data.
     * @param name - a title for the plot data.
     */
    void setName( const QString& name );

    virtual ~Plot2DResult(){}

  protected:
      QString m_name;
      QString m_unitsX;
      QString m_unitsY;
      std::vector<std::pair<double,double>> m_data;
};
}
}
}
