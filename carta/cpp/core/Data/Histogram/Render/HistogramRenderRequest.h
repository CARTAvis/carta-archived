/**
 * Encapsulation of the parameters that specify how a histogram should be
 * computed.
 */

#pragma once

#include <memory>
#include <QString>

namespace Carta{
namespace Lib {
namespace Image {
	class ImageInterface;
}
namespace Regions {
	class RegionBase;
}
}
namespace Data{

class Layer;
class Region;

class HistogramRenderRequest {

public:

	/**
	 * Constructor.
	 * @param dataSource - the image to use in making the histogram.
	 * @param binCount - the number of histogram bins.
	 * @param minChannel - the minimum frame index.
	 * @param maxChannel - the maximum frame index.
	 * @param minFrequency - the minimum frequency.
	 * @param maxFrequency - the maximum frequency.
	 * @param rangeUnits - intensity units.
	 * @param minIntensity - the minimum intensity value.
	 * @param maxIntensity - the maximum intensity value.
	 * @param fileName - an identifier for the image.
	 * @param region - a 2D extent of the region or null if the histogram is to be of the entire image.
	 * @param regionId - an identifier for the region.
	 */
	explicit HistogramRenderRequest( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
			int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
			const QString& rangeUnits, double minIntensity, double maxIntensity,
			const QString& fileName, std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
			const QString& regionId);

	/**
	 * Return the number of histogram bins.
	 * @return - a count of the histogram bins.
	 */
	int getBinCount() const;

	/**
	 * Return the maximum channel to include in the histogram or -1 if there is no maximum.
	 * @return - the maximum channel in the image.
	 */
	int getChannelMax() const;

	/**
	 * Return the minimum channel index to include in the histogram or -1 if there is no minimum.
	 * @return - the minimum channel in the image.
	 */
	int getChannelMin() const;

	/**
	 * Return a unique identifier for the image.
	 * @return - a unique identifier for the image.
	 */
	QString getFileName() const;

	/**
	 * Return a unique identifier for the render request.
	 * @return - unique identifier for the request.
	 */
	QString getId() const;

	/**
	 * Return the image.
	 * @return - the image.
	 */
	std::shared_ptr<Carta::Lib::Image::ImageInterface> getImage() const;

	/**
	 * Get the maximum intensity value for the histogram.
	 * @return - the maximum intensity value for the histogram.
	 */
	int getIntensityMax() const;

	/**
	 * Get the minimum intensity value for the histogram.
	 * @return - the minimum intensity value for the histogram.
	 */
	int getIntensityMin() const;

	/**
	 * Return the maximum frequency for the histogram.
	 * @return - the maximum frequency for the histogram.
	 */
	int getFrequencyMax() const;

	/**
	 * Return the minimum frequency for the histogram.
	 * @return - the minimum frequency for the histogram.
	 */
	int getFrequencyMin() const;

	/**
	 * Return intensity units.
	 * @return - intensity units.
	 */
	QString getRangeUnits() const;

	/**
	 * Returns the region to be profiled.
	 * @return - the region to be profiled.
	 */
	std::shared_ptr<Carta::Lib::Regions::RegionBase> getRegion() const;

	/**
	 * Returns an identifier for the region over which the histogram will be computed.
	 * @return - an identifier for the region over which the the histogram will be computed.
	 */
	QString getRegionId() const;

	/**
	 * Returns whether or not the other request is equal to this one.
	 * @param other - a potentially different request to render a profile.
	 * @return - true, if the other request is the same as this one; false, otherwise.
	 */
	bool operator==( const HistogramRenderRequest& other );

	/**
	 * Destructor.
	 */
	~HistogramRenderRequest();

private:

	std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image;
	std::shared_ptr<Carta::Lib::Regions::RegionBase> m_region;
	int m_binCount;
	int m_minChannel;
	int m_maxChannel;
	double m_minFrequency;
	double m_maxFrequency;
	double m_minIntensity;
	double m_maxIntensity;
	QString m_rangeUnits;
	QString m_fileName;
	QString m_regionId;

};
}
}



