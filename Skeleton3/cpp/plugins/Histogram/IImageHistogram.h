/*
 *  Functionality expected of a histogram implementation.
 */

#ifndef IIMAGEHISTOGRAM_H_
#define IIMAGEHISTOGRAM_H_

#include <vector>

// namespace casa {
//     template <class T> class ImageInterface;
//     template <class T> class LatticeHistograms;
//     template <class T> class SubImage;
//     class ImageRegion;
// }

/**
 * Generates and Manages the data corresponding to a histogram.
 */
//template <class T>

class IImageHistogram{
public:

    /* Recomputes the histogram data.
     * @return true if the data was successfully generated; false otherwise.
     */
    virtual bool compute() = 0;

    /**
     * Returns the histogram data as (intensity,count) pairs.
     * @return a list of (intensity,count) data pairs.
     */
    virtual std::vector< std::pair<double,double> > getData() const = 0;
    virtual void setBinCount( int count ) = 0;
    virtual void setChannelRange( int minChannel, int maxChannel, int spectralIndex=-1 )=0;
    virtual void setIntensityRange( double minimumIntensity, double maximumIntensity )=0;
    //virtual void setImage(casa::ImageInterface<T> *  val)=0;

protected:
    virtual ~IImageHistogram();
    IImageHistogram();
};

#endif /* IIMAGEHISTOGRAM_H_ */
