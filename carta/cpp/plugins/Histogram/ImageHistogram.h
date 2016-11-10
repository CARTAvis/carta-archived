#pragma once

#include <casacore/casa/vector.h>
#include "IImageHistogram.h"
#include <QTextStream>


#include <memory>

namespace casa {
    template <class T> class ImageInterface;
    template <class T> class LatticeHistograms;
    template <class T> class SubImage;
    class ImageRegion;
}

/**
 * Generates and Manages the data corresponding to a histogram.
 */
template <class T>
class ImageHistogram : public IImageHistogram {
public:
	ImageHistogram();

    virtual std::vector< std::pair<double,double> > getData() const Q_DECL_OVERRIDE;
    virtual QString getName() const Q_DECL_OVERRIDE;
    virtual QString getUnitsX() const Q_DECL_OVERRIDE;
    virtual QString getUnitsY() const Q_DECL_OVERRIDE;

    virtual bool compute() Q_DECL_OVERRIDE;

	int getDataCount() const;
	void setRegion(casa::ImageRegion* region, const QString& id );
	void defineLine( int index, QVector<double>& xVals, QVector<double>& yVals,
			bool useLogY ) const;
	void defineStepHorizontal( int index, QVector<double>& xVals, QVector<double>& yVals,
			bool useLogY ) const;
	void defineStepVertical( int index, QVector<double>& xVals, QVector<double>& yVals,
			bool useLogY ) const;
	std::pair<float,float> getMinMaxBinCount() const;
	vector<T> getXValues() const;
	vector<T> getYValues() const;

	std::pair<float,float> getDataRange() const;
	void toAscii( QTextStream& out ) const;


	//common to all histograms
	void setBinCount( int count )  Q_DECL_OVERRIDE;
	void setChannelRangeDefault();
	void setIntensityRangeDefault();
	void setChannelRange( int minChannel, int maxChannel )  Q_DECL_OVERRIDE;

	void setIntensityRange( double minimumIntensity, double maximumIntensity )  Q_DECL_OVERRIDE;

	void setImage(const casa::ImageInterface<T>*  val);
	static double computeYValue( double value, bool useLog );


	virtual ~ImageHistogram();

private:
	ImageHistogram( const ImageHistogram<T>& other );
	ImageHistogram operator=( const ImageHistogram<T>& other );
	//Completely reset the histogram if the image, region, or channels change
	bool _reset();
	void _filterByChannels( const casa::ImageInterface<T>*  image );

	vector<T> m_xValues;
	vector<T> m_yValues;
	casa::LatticeHistograms<T>* m_histogramMaker;
	casa::ImageRegion* m_region;
	const int ALL_CHANNELS;
	const int ALL_INTENSITIES;
    const casa::ImageInterface<T>*  m_image; //Use
	int m_channelMin;
	int m_channelMax;
	double m_intensityMin;
	double m_intensityMax;
	int m_binCount;
	QString m_regionId;
};
