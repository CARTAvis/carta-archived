#ifndef COORDINATESYSTEM_H_
#define COORDINATESYSTEM_H_

/**
 * Note:  Casacore coordinate system is badly implemented.
 * 1.  A CoordinateSystem is not a Coordinate
 * 2.  Mixes enums and inheritance.
 *
 */
//Pure virtual class.  Things plug-ins need.
class CoordinateSystem {
public:
	virtual ~CoordinateSystem();

};

/**
 * Wrapper class for the casa coordinate system that implements
 * the CoordinateSytem interface.
 */
class CoordinateSystemImpl : public CoordinateSystem {

public:
	CoordinateSystemImpl();
	virtual ~CoordinateSystemImpl();
private:
	//Wrapped casacore coordinate system.  Eventually to be replaced
	//with a better implementation.
	CoordinateSystem* m_casaCoordSys;
};

#endif /* COORDINATESYSTEM_H_ */
