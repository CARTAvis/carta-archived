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
	 * @param name - the name of the region.
	 * @return - the selected region.
	 */
	std::shared_ptr<Region> getRegion( const QString& name ) const;

	/**
	 * Return the number of regions.
	 * @return - the number of regions.
	 */
	int getRegionCount() const;

	/**
	 * Return the managed regions.
	 * @return - the managed regions.
	 */
	std::vector<std::shared_ptr<Region> > getRegions() const;

	/**
	 * Get the index of the selected region.
	 * @return - the index of the selected region.
	 */
	int getIndexCurrent() const;

	/**
	 * Return whether or not regions are being selected automatically.
	 * @return - true if regions are being selected automatically; false otherwise.
	 */
	bool isAutoSelect() const;

	/**
	 * Force a state refresh.
	 */
	virtual void refreshState() Q_DECL_OVERRIDE;

	/**
	 * Reset the state based on the string passed in.
	 * @param state - the state to restore.
	 */
	void resetStateString( const QString& state );

	/**
	 * Set whether or not to auto select regions based on the animator.
	 * @param autoSelect - true if regions should be automatically selected;
	 * 		false otherwise.
	 */
	void setAutoSelect( bool autoSelect );

	/**
	 * Set the index of the current regions.
	 * @param index - the index of the current regions.
	 */
	void setIndexCurrent( int index );

	/**
	 * Set the center of all selected regions.
	 * @param center - the center of all selected regions.
	 */
	void setRegionCenter( const QPointF& center );

	/**
	 * Create a region of the indicated type.
	 * @param createType - an identifier for the type of region to create.
	 * @return - an error message if the region could not be created; otherwise,
	 * 		and empty string.
	 */
	QString setRegionCreateType( const QString& createType );

	/**
	 * Set the height of all selected regions.
	 * @param height - the new height of the selected regions.
	 * @return - an error message if the height could not be set; otherwise, an
	 * 			empty string.
	 */
	virtual QString setRegionHeight( double height );

	/**
	 * Set the major radius of all selected regions (where a major radius makes sense).
	 * @param radius - the new radius for all selected regions.
	 * @return - an error message if the new radius could not be set; otherwise, an
	 * 		empty string.
	 */
	QString setRegionRadiusMajor( double radius );

	/**
	 * Set the minor radius of all selected regions (where a minor radius makes sense).
	 * @param radius - the new radius for all selected regions.
	 * @return - an error message if the new radius could not be set; otherwise, an
	 * 		empty string.
	 */
	QString setRegionRadiusMinor( double radius );

	/**
	 * Set the width of all selected regions.
	 * @param width - the new width of the selected regions.
	 * @return - an error message if the width could not be set; otherwise, an
	 * 			empty string.
	 */
	virtual QString setRegionWidth( double width );

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

	void _indexChanged();

	void _regionSelectionChanged( const QString& id );

	void _regionShapeChanged();

private:

	RegionControls (const QString& path, const QString& id );
	class Factory;

	void _addDataRegions( std::vector<std::shared_ptr<Region>> regions );

	int _findRegionIndex( const QString& id ) const;

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

	/**
	 * Reset the state back to a different state.
	 * @param state - a different state.
	 */
	void _resetStateData( const QString& state );

	void _saveStateRegions();
	void _setRegionsSelected( QStringList ids );

	RegionControls( const RegionControls& other);
	RegionControls& operator=( const RegionControls& other );

	//Data State
	Carta::State::StateInterface m_stateData;

	//Selection for regions
	Selection* m_selectRegion;

	//The regions
	std::vector<std::shared_ptr<Region> > m_regions;
	std::shared_ptr<Region> m_regionEdit;
	static RegionTypes* m_regionTypes;

	static const QString CREATE_TYPE;
	static const QString REGIONS;
	static const QString REGION_INDEX;
	static const QString REGION_SELECT_AUTO;

	static bool m_registered;
};
}
}
