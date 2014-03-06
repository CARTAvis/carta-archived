#ifndef NDARRAY_H_
#define NDARRAY_H_

/**
 * Should work for small array sizes.  Should be able to support
 * any scalar type.  Very similar to casacore array without template.
 */

class NDArray {
public:
	NDArray();

	virtual ~NDArray();
};

#endif /* NDARRAY_H_ */
