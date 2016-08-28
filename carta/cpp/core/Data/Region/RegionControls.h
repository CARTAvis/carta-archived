/***
 * Manager for the regions that have been initialized.
 */

#pragma once
#include "Shape/IShape.h"
#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include <memory>

namespace Carta {
namespace Data {

class Selection;
class RegionTypes;
class Region;

typedef Carta::Lib::InputEvents::JsonEvent InputEvent;

class RegionControls : public QObject, public Carta::State::CartaObject {

	friend class Controller;
	friend class DataFactory;

	Q_OBJECT

public:

	/**
	 * Close a particular region.
	 * @param index - the index of the region to close.
	 */
	QString closeRegion( int index );

	/**
	 * Return the selected region.
	 * @return - the selected region.
	 */
	std::shared_ptr<Region> getRegion() const;

	/**
	 * Return the managed regions.
	 * @return - the managed regions.
	 */
	std::vector<std::shared_ptr<Region> > getRegions() const;

	/**
	 * Get the index of the selected region.
	 * @return - the index of the selected region.
	 */
	int getSelectRegionIndex() const;

	/**
	 * Return the vector graphics for all the managed regions.
	 * @return - the vector graphics for all the managed regions.
	 */
	Carta::Lib::VectorGraphics::VGList vgList() const;

	virtual ~RegionControls();

	const static QString CLASS_NAME;

signals:

	/**
	 *  Notification that the region set has been updated.
	 */
	void regionsChanged( );

private:

	RegionControls (const QString& path, const QString& id );
	class Factory;

	void _addDataRegions( std::vector<std::shared_ptr<Region>> regions );

	int _findRegionIndex( std::shared_ptr<Region> region ) const;

	/**
	 * Return the region state as a string.
	 * @return - the region state as a string.
	 */
	QString _getStateString() const;

	void _initializeCallbacks();
	void _initializeSelections();
	void _initializeState();
	void _initializeStatics();

	bool _handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev );
	bool _handleHover( const Carta::Lib::InputEvents::HoverEvent& ev );
	bool _handleTap( const Carta::Lib::InputEvents::DoubleTapEvent& ev );
	bool _handleTouch( const Carta::Lib::InputEvents::TouchEvent& ev );

	void _onInputEvent(const InputEvent & ev );

	void _saveStateRegions();

	QString _setRegionCreateType( const QString& createType );

	RegionControls( const RegionControls& other);
	RegionControls& operator=( const RegionControls& other );

	//Selection for regions
	Selection* m_selectRegion;

	//The type of region to create.
	QString m_regionCreateType;

	//The regions
	std::vector<std::shared_ptr<Region> > m_regions;

	static RegionTypes* m_regionTypes;
	static const QString REGIONS;
	static bool m_registered;
};
}
}
