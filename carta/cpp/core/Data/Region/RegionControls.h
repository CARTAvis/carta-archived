/***
 * Manager for the regions that have been initialized.
 */

#pragma once
#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"
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
	 * Remove existing regions.
	 */
	void clearRegions();

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
	 * Reset the state back to a different state.
	 * @param state - a different state.
	 */
	void resetStateData( const QString& state );

	/**
	 * Create a region of the indicated type.
	 * @param createType - an identifier for the type of region to create.
	 * @return - an error message if the region could not be created; otherwise,
	 * 		and empty string.
	 */
	QString setRegionCreateType( const QString& createType );

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


private slots:

	void _editDone();

private:

	RegionControls (const QString& path, const QString& id );
	class Factory;

	void _addDataRegions( std::vector<std::shared_ptr<Region>> regions );

	int _findRegionIndex( std::shared_ptr<Region> region ) const;

	QString _getStateString( const QString& sessionId, SnapshotType type ) const;

	void _initializeCallbacks();
	void _initializeSelections();
	void _initializeState();
	void _initializeStatics();

	bool _handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev, const QPointF& imagePt );
	bool _handleHover( const Carta::Lib::InputEvents::HoverEvent& ev, const QPointF& imagePt );
	bool _handleTapDouble( const Carta::Lib::InputEvents::DoubleTapEvent& ev, const QPointF& imagePt );
	bool _handleTouch( const Carta::Lib::InputEvents::TouchEvent& ev, const QPointF& imagePt );

	void _onInputEvent( InputEvent & ev, const QPointF& imagePt );

	void _saveStateRegions();

	RegionControls( const RegionControls& other);
	RegionControls& operator=( const RegionControls& other );

	//Selection for regions
	Selection* m_selectRegion;

	//The regions
	std::vector<std::shared_ptr<Region> > m_regions;
	std::shared_ptr<Region> m_regionEdit;
	static RegionTypes* m_regionTypes;
	static const QString REGIONS;
	static const QString CREATE_TYPE;
	static bool m_registered;
};
}
}
