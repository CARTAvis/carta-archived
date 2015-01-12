/*
 *  Functionality expected of a histogram implementation.
 */

#ifndef IIMAGEHISTOGRAM_H_
#define IIMAGEHISTOGRAM_H_

#include <vector>

class IImageHistogram {
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

protected:
    virtual ~IImageHistogram();
    IImageHistogram();
};

#endif /* IIMAGEHISTOGRAM_H_ */
