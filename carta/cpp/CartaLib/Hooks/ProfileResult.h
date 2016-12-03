/**
 * Stores profile data and associated information needed to display a profile and
 * information about the profile.
 */
#pragma once
#include <QString>
#include <vector>
#include "CartaLib/ProfileInfo.h"

namespace Carta{
namespace Lib{

namespace Hooks {

class ProfileResult {
  

  public:
  	ProfileResult( double imageRest = 0, const QString& restUnits = "",
  		const std::vector< std::pair<double, double> > data = std::vector< std::pair<double,double> >());

  	/**
  	 * Return (x,y) data pairs that comprise a profile.
  	 * @return - (x,y) data pairs that comprise a profile curve.
  	 */
  	std::vector< std::pair<double,double> > getData() const;

  	/**
  	 * Return information about any errors that prevented the computation of a profile.
  	 * @return - an error message or an empty string if no errors were encountered.
  	 */
    QString getError() const;

  	/**
  	 * Returns the image rest frequency units.
  	 * @return - the image rest frequency units.
  	 */
  	QString getRestUnits() const;

  	/**
  	 * Returns the image rest frequency.
  	 * @return - the image rest frequency.
  	 */
  	double getRestFrequency() const;

  	/**
  	 * Store the (x,y) data pairs that comprise a profile.
  	 * @param data - the (x,y) data pairs that make up a profile.
  	 */
  	void setData( const std::vector< std::pair<double,double> >& data );

  	/**
  	 * Set an error message if there was a problem computing the profile.
  	 * @param errorMessage - a description of the problem.
  	 */
  	void setError( const QString& errorMsg );

  	/**
  	 * Store the image rest frequency.
  	 * @param restFreq - the image rest frequency.
  	 */
  	void setRestFrequency( double restFreq );

  	/**
  	 * Store the image rest units.
  	 * @param restUnits - the image rest units.
  	 */
  	void setRestUnits( const QString& restUnits );

    virtual ~ProfileResult(){}

  private:
      std::vector< std::pair<double,double> > m_data;
      double m_restFrequency;
      QString m_restUnits;
      QString m_errorMessage;
};

//Serialization so that the profile result can be generated in a separate process.
QDataStream &operator<<(QDataStream& out, const Carta::Lib::Hooks::ProfileResult& result );
QDataStream &operator>>(QDataStream& in, Carta::Lib::Hooks::ProfileResult& result );

}
}
}
