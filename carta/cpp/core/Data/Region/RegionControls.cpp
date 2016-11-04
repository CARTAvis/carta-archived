#include "RegionControls.h"
#include "Region.h"
#include "RegionFactory.h"
#include "RegionTypes.h"
#include "Data/Util.h"
#include "Data/Selection.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Regions/Ellipse.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionControls::CLASS_NAME = "RegionControls";
const QString RegionControls::CREATE_TYPE = "createType";
const QString RegionControls::REGIONS = "regions";
const QString RegionControls::REGION_INDEX = "regionIndex";
const QString RegionControls::REGION_SELECT_AUTO = "regionAutoSelect";

RegionTypes* RegionControls::m_regionTypes = nullptr;

class RegionControls::Factory : public Carta::State::CartaObjectFactory {
public:

	Factory():
		CartaObjectFactory(CLASS_NAME){
	};

	Carta::State::CartaObject * create (const QString & path, const QString & id)
	{
		return new RegionControls(path, id);
	}
};

bool RegionControls::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionControls::Factory());


RegionControls::RegionControls(const QString& path, const QString& id )
:CartaObject( CLASS_NAME, path, id ),
 	 m_stateData( Carta::State::UtilState::getLookup(path, Carta::State::StateInterface::STATE_DATA)),
 	 m_selectRegion( nullptr ),
	 m_regionEdit( nullptr ){
	_initializeStatics();
	_initializeSelections();
	_initializeCallbacks();
	_initializeState();
}

void RegionControls::_addDataRegions( std::vector<std::shared_ptr<Region>> regions ){
	int count = regions.size();
	for ( int i = 0; i < count; i++ ){
		const QString id = regions[i]->getId();
		int regionIndex = _findRegionIndex( id );
		if ( regionIndex < 0 ){
			regions[i]->setEditMode( false );
			m_regions.push_back( regions[i]);
			int regionIndex = m_regions.size() - 1;
			connect( m_regions[regionIndex].get(), SIGNAL(regionSelectionChanged(const QString&)),
					this, SLOT(_regionSelectionChanged( const QString&)));
			connect( m_regions[regionIndex].get(), SIGNAL(regionShapeChanged()),
					this, SLOT(_regionShapeChanged()));
		}
	}
    count = m_regions.size();

    m_selectRegion->setUpperBound( count );
    if ( count >= 1 ){
    	int selectIndex = count - 1;
    	m_selectRegion->setIndex( selectIndex );
    }
    _saveStateRegions();
    emit regionsChanged();
}

void RegionControls::clearRegions(){
	int regionCount = m_regions.size();
	for ( int i = regionCount - 1; i>= 0; i-- ){
		closeRegion( i );
	}
}


QString RegionControls::closeRegion( int index ){
    bool regionRemoved = false;
    QString result;
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    //Note that more than one region could be removed, if there are
    //several regions that start with the passed in id.
    int regionCount = m_regions.size();
    if ( index >= 0 && index < regionCount ){
        QString id = m_regions[index]->getId();
        objMan->removeObject( id );
        m_regions.erase( m_regions.begin() + index );
        regionRemoved = true;
    }
    if ( regionRemoved ){
    	int selectedRegion = m_selectRegion->getIndex();
    	int regionCount = m_regions.size();
    	m_selectRegion->setUpperBound( regionCount );
    	if ( selectedRegion >= regionCount ){
    		m_selectRegion->setIndex(regionCount - 1);
    	}
    	_saveStateRegions();
    	emit regionsChanged();
    }
    else {
        result = "Could not find region to remove for index="+QString::number(index);
    }
    return result;
}


void RegionControls::_editDone(){
	if ( m_regionEdit ){
		disconnect( m_regionEdit.get(), & Region::editDone,
		             this, & RegionControls::_editDone );
		m_regionEdit->setEditMode( false );

		m_regionEdit->setActive( true );
		m_regions.push_back( m_regionEdit );
		int regionCount = m_regions.size();
		connect( m_regions[regionCount-1].get(), SIGNAL(regionSelectionChanged(const QString&)),
				this, SLOT(_regionSelectionChanged( const QString&)));
		connect( m_regions[regionCount-1].get(), SIGNAL(regionShapeChanged()),
				this, SLOT(_regionShapeChanged()));

		m_selectRegion->setUpperBound( m_regions.size() );
		m_regionEdit = std::shared_ptr<Region>(nullptr);
		m_state.setValue<QString>(CREATE_TYPE, "");
		_saveStateRegions();
		m_state.flushState();
	}
}


int RegionControls::_findRegionIndex( const QString& id ) const {
	int index = -1;
	int regionCount = m_regions.size();
	for ( int i = 0; i < regionCount; i++ ){
		QString otherId = m_regions[i]->getId();
		if ( id == otherId ){
			index = i;
			break;
		}
	}
	return index;
}

std::shared_ptr<Region> RegionControls::getRegion( const QString& regionName ) const {
    std::shared_ptr<Region> region( nullptr );
    int regionIndex = -1;
    //Used the current region if the passed in id is empty
    if ( regionName.isEmpty() || regionName.length() == 0 ){
		regionIndex = m_selectRegion->getIndex();
    }
    else {
    	//Find the region with matching id.
    	int regionCount = m_regions.size();
    	for ( int i = 0; i < regionCount; i++ ){
    		if ( m_regions[i]->getRegionName() == regionName ){
    			regionIndex = i;
    			break;
    		}
    	}
    }
    int regionCount = m_regions.size();
    if ( regionIndex >= 0 && regionIndex < regionCount ){
    	region = m_regions[regionIndex];
    }
    return region;
}

int RegionControls::getRegionCount() const {
	return m_regions.size();
}

std::vector<std::shared_ptr<Region> > RegionControls::getRegions() const {
    return m_regions;
}


int RegionControls::getIndexCurrent() const {
    int selectRegionIndex = -1;
    if ( m_regions.size() > 0 ){
        selectRegionIndex = m_selectRegion->getIndex();
    }
    return selectRegionIndex;
}

QString RegionControls::_getStateString( const QString& /*sessionId*/, SnapshotType type ) const {
	QString result;
	if ( type == SNAPSHOT_PREFERENCES ){
		result = m_state.toString();
	}
	else if ( type == SNAPSHOT_DATA ){
		Carta::State::StateInterface state( "" );
		state.setState( m_stateData.toString() );
		state.insertValue<QString>( Selection::REGION, m_selectRegion->getStateString());
		result = state.toString();
	}
	return result;
}


bool RegionControls::_handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev, const QPointF& imagePt ){
	bool validDrag = false;
	if ( ev.isValid() ){
		validDrag = true;
		if ( m_regionEdit ){
			m_regionEdit->handleDrag( ev, imagePt );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleDrag( ev, imagePt );
			}
			if ( regionCount > 0 ){
				emit regionsChanged();
			}
		}
	}
	return validDrag;
}

bool RegionControls::_handleHover( const Carta::Lib::InputEvents::HoverEvent& ev,
		const QPointF& imagePt ){
	bool validHover = false;
	if ( ev.isValid() ){
		validHover = true;
		if ( m_regionEdit ){
			m_regionEdit->handleHover( imagePt );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleHover( imagePt );
			}
			if ( regionCount > 0 ){
				emit regionsChanged();
			}
		}

	}
	return validHover;
}


bool RegionControls::_handleTouch( const Carta::Lib::InputEvents::TouchEvent& ev,
		const QPointF& imagePt ){
	bool validTap = false;
	if ( ev.isValid() ){
		validTap = true;
		if ( m_regionEdit ){
			m_regionEdit->handleTouch( imagePt );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			int selectedIndex =m_selectRegion->getIndex();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleTouch( imagePt);
			}
			if ( regionCount > 0 ){
				//If it was a tap outside all of the regions, it may have unselected
				//all of them, but the current region should be selected.
				m_regions[selectedIndex]->setSelected( true );
				emit regionsChanged();
			}
		}
	}
	return validTap;
}

bool RegionControls::_handleTapDouble( const Carta::Lib::InputEvents::DoubleTapEvent& ev, const QPointF& imagePt ){
	bool validTap = false;
	if ( ev.isValid() ){
		if ( m_regionEdit ){
			validTap = true;
			m_regionEdit->handleTapDouble( imagePt );
			emit regionsChanged();
		}
	}
	return validTap;
}


void RegionControls::_initializeCallbacks(){
	addCommandCallback( "closeRegion", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {"region"};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString regionIdStr = dataValues[*keys.begin()];
		bool validInt = false;
		int regionId = regionIdStr.toInt( &validInt );
		QString result;
		if ( validInt ){
			result = closeRegion( regionId );
		}
		else {
			result="Index of region to close must be an integer";
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setAutoSelect", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {"autoSelect"};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString autoStr = dataValues[*keys.begin()];
		bool validBool = false;
		bool autoSelect = Util::toBool( autoStr, &validBool );
		QString result;
		if ( validBool ){
			setAutoSelect( autoSelect );
		}
		else {
			result="Whether or not to auto select regions must be true/false: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setCenter", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {Util::XCOORD, Util::YCOORD};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString xStr = dataValues[Util::XCOORD];
		QString yStr = dataValues[Util::YCOORD];
		bool validDoubleX = false;
		double xVal = xStr.toDouble(&validDoubleX);
		bool validDoubleY = false;
		double yVal = yStr.toDouble(&validDoubleY);
		QString result;
		if ( validDoubleX && validDoubleY ){
			setRegionCenter( QPointF(xVal,yVal) );
		}
		else {
			result="The coordinates of the region center must be numbers: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setHeight", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {Util::HEIGHT};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString heightStr = dataValues[*keys.begin()];
		bool validDouble = false;
		double height = heightStr.toDouble(&validDouble);
		QString result;
		if ( validDouble ){
			setRegionHeight( height );
		}
		else {
			result="The height of the selected region must be a number: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setRadiusMajor", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {Carta::Lib::Regions::Ellipse::RADIUS_MAJOR};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString radiusStr = dataValues[*keys.begin()];
		bool validDouble = false;
		double radius = radiusStr.toDouble(&validDouble);
		QString result;
		if ( validDouble ){
			result = setRegionRadiusMajor( radius );
		}
		else {
			result="The major radius of the selected region must be a number: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setRadiusMinor", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {Carta::Lib::Regions::Ellipse::RADIUS_MINOR};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString radiusStr = dataValues[*keys.begin()];
		bool validDouble = false;
		double radius = radiusStr.toDouble(&validDouble);
		QString result;
		if ( validDouble ){
			result = setRegionRadiusMinor( radius );
		}
		else {
			result="The minor radius of the selected region must be a number: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setRegionsSelected", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) -> QString {
		QString result;
		QStringList names = params.split(";");
		if ( names.size() == 0 ){
			result = "Please specify the regions to select.";
		}
		else {
			_setRegionsSelected( names );
		}
		Util::commandPostProcess( result );
		return result;
	});

	addCommandCallback( "setWidth", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) ->QString {
		std::set<QString> keys = {Util::WIDTH};
		std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
		QString widthStr = dataValues[*keys.begin()];
		bool validDouble = false;
		double width = widthStr.toDouble(&validDouble);
		QString result;
		if ( validDouble ){
			setRegionWidth( width );
		}
		else {
			result="The width of the selected region must be a number: "+params;
		}
		Util::commandPostProcess( result );
		return result;
	});
}

void RegionControls::_initializeSelections(){
	Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
	m_selectRegion = objMan->createObject<Selection>();
	connect( m_selectRegion, SIGNAL(indexChanged()), this, SLOT(_indexChanged()));
}

void RegionControls::_indexChanged(){
	int index = m_selectRegion->getIndex();
	m_stateData.setValue<int>( REGION_INDEX, index );
	m_stateData.flushState();
}

void RegionControls::_initializeState(){
    int regionCount = m_regions.size();
    m_stateData.insertValue<int>( REGION_INDEX, 0 );
    m_stateData.insertArray(REGIONS, regionCount );
    m_stateData.flushState();

    m_state.insertValue<QString>( CREATE_TYPE, "" );
    m_state.insertValue<bool>(REGION_SELECT_AUTO, true );
    m_state.insertValue<int>(Util::SIGNIFICANT_DIGITS, Region::SIGNIFICANT_DIGITS );
    m_state.flushState();
}


void RegionControls::_initializeStatics(){
    if ( m_regionTypes == nullptr ){
        m_regionTypes = Util::findSingletonObject<RegionTypes>();
    }
}

bool RegionControls::isAutoSelect() const {
	return m_state.getValue<bool>( REGION_SELECT_AUTO );
}

void RegionControls::_onInputEvent(InputEvent & ev, const QPointF& imagePt ){
	if ( !_handleHover( Carta::Lib::InputEvents::HoverEvent( ev ), imagePt ) ) {
		if ( !_handleTapDouble( Carta::Lib::InputEvents::DoubleTapEvent( ev ), imagePt ) ) {
			if ( !_handleTouch( Carta::Lib::InputEvents::TouchEvent( ev ), imagePt ) ){
				if( !_handleDrag( Carta::Lib::InputEvents::Drag2Event(ev), imagePt) ){
					//qWarning() << "RegionControls:: unhandled event: "<<ev.type();
				}
			}
		}
		else {
			//Note that we are doing this so if we are editing a polygon region and
			//closing it with a double click, the image will not also be panned.
			ev.setConsumed();
		}
	}
}

void RegionControls::refreshState(){
	CartaObject::refreshState();
	m_stateData.refreshState();
}


void RegionControls::_regionSelectionChanged( const QString& id ){
	if ( !isAutoSelect() ){
		int regionIndex = _findRegionIndex( id );
		if ( regionIndex >= 0 ){
			if ( m_regions[regionIndex]->isSelected() ){
				setIndexCurrent( regionIndex );
			}
		}
	}
	_saveStateRegions();
}

void RegionControls::_regionShapeChanged( ){
	_saveStateRegions();
}

void RegionControls::_resetStateData( const QString& state ){
    Carta::State::StateInterface dataState( "");
    dataState.setState( state );

    clearRegions();
    int regionCount = dataState.getArraySize( REGIONS );
    m_regions.resize( regionCount );
    for ( int i = 0; i < regionCount; i++ ){
    	QString regionKey = Carta::State::UtilState::getLookup( REGIONS, i);
    	QString regionValue = dataState.toString( regionKey );
    	m_regions[i] = RegionFactory::makeRegion( regionValue );
    	connect( m_regions[i].get(), SIGNAL(regionSelectionChanged(const QString&)),
    					this, SLOT(_regionSelectionChanged( const QString&)));
    	connect( m_regions[i].get(), SIGNAL(regionShapeChanged()),
    					this, SLOT(_regionShapeChanged()));
    }
    int regionIndex = dataState.getValue<int>(REGION_INDEX);
    m_stateData.setValue<int>(REGION_INDEX, regionIndex);
    QString selectStateStr = dataState.getValue<QString>( Selection::REGION );
    m_selectRegion ->resetState( selectStateStr );
    _saveStateRegions();
    emit regionsChanged();
}

void RegionControls::resetStateString( const QString& stateStr ){
	Carta::State::StateInterface state( "");
	state.setState( stateStr );
	m_state.setValue<bool>(REGION_SELECT_AUTO, state.getValue<bool>(REGION_SELECT_AUTO) );
	m_state.setValue<QString>(CREATE_TYPE, state.getValue<QString>(CREATE_TYPE));

	m_state.flushState();
}

void RegionControls::_saveStateRegions(){
    //Regions
    int regionCount = m_regions.size();
    int oldRegionCount = m_stateData.getArraySize( REGIONS);
    if ( regionCount != oldRegionCount){
        m_stateData.resizeArray( REGIONS, regionCount, Carta::State::StateInterface::PreserveNone );
    }
    for ( int i = 0; i < regionCount; i++ ){
        QString regionKey = Carta::State::UtilState::getLookup( REGIONS, i);
        QString regionTypeStr= m_regions[i]->_getStateString();
        m_stateData.setObject( regionKey, regionTypeStr );
    }
    m_stateData.flushState();
}

void RegionControls::setAutoSelect( bool autoSelect ){
	bool oldSelect = m_state.getValue<bool>( REGION_SELECT_AUTO );
	if ( oldSelect != autoSelect ){
		m_state.setValue<bool>( REGION_SELECT_AUTO, autoSelect );
		m_state.flushState();
	}
}

void RegionControls::setIndexCurrent( int index ){
	int oldIndex = m_selectRegion->getIndex();
	m_selectRegion->setIndex( index );
	if ( oldIndex != index ){
		_saveStateRegions();
		emit regionsChanged();
	}
}

void RegionControls::setRegionCenter( const QPointF& center ){
	bool centerChanged = false;
	int regionCount = m_regions.size();
	for ( int i = 0; i < regionCount; i++ ){
		if ( m_regions[i]->isSelected() ){
			if ( m_regions[i]->setCenter( center ) ){
				centerChanged = true;
			}
		}
	}
	if ( centerChanged ){
		_saveStateRegions();
		emit regionsChanged();
	}
}


QString RegionControls::setRegionCreateType( const QString& createType ){
	QString result;
	QString modelType = m_regionTypes->getModelType( createType );
	if ( !modelType.isEmpty() ){

		//We already have one of the correct type.
		QString actualType = m_regionTypes->getActualType( createType );
		QString oldCreateType = m_state.getValue<QString>( CREATE_TYPE );
		if ( actualType != oldCreateType ){
			//Clear the region edit if it exists and is not of the correct type.
			if ( m_regionEdit && m_regionEdit->getRegionType() != actualType ){
				disconnect( m_regionEdit.get() );
				QString id = m_regionEdit->getId();
				Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
				objMan->removeObject( id );
				m_regionEdit = nullptr;
			}
			//Make a new region to edit.
			if ( !m_regionEdit ){
				m_regionEdit = RegionFactory::makeRegionType( modelType );
				m_regionEdit->setEditMode( true );
				connect( m_regionEdit.get(), SIGNAL(editDone()), this, SLOT(_editDone()));
			}

			m_state.setValue<QString>( CREATE_TYPE, actualType );
			m_state.flushState();
		}
	}
	else {
		result = "Unrecognized type of region to draw "+createType;
	}
	return result;
}

QString RegionControls::setRegionHeight( double height ){
	QString result;

	if ( height >= 0 ){
		bool heightChanged = false;
		int regionCount = m_regions.size();
		for ( int i = 0; i < regionCount; i++ ){
			if ( m_regions[i]->isSelected() ){
				if ( m_regions[i]->setHeight( height ) ){
					heightChanged = true;
				}
			}
		}
		if ( heightChanged ){
			_saveStateRegions();
			emit regionsChanged();
		}
	}
	else {
		result = "Height of region must be nonnegative: "+QString::number( height );
	}
	return result;
}

QString RegionControls::setRegionRadiusMajor( double radius ){
	QString result;
	if ( radius >= 0 ){
		bool radiusChanged = false;
		int regionCount = m_regions.size();
		for ( int i = 0; i < regionCount; i++ ){
			if ( m_regions[i]->isSelected() ){
				bool changed = false;
				result = m_regions[i]->setRadiusMajor( radius, &changed );
				if ( changed ){
					radiusChanged = true;
				}
			}
		}
		if ( radiusChanged ){
			_saveStateRegions();
			emit regionsChanged();
		}
	}
	else {
		result = "Major radius of region must be nonnegative: "+QString::number( radius );
	}
	return result;
}

QString RegionControls::setRegionRadiusMinor( double radius ){
	QString result;
	if ( radius >= 0 ){
		bool radiusChanged = false;
		int regionCount = m_regions.size();
		for ( int i = 0; i < regionCount; i++ ){
			if ( m_regions[i]->isSelected() ){
				bool changed = false;
				result = m_regions[i]->setRadiusMinor( radius, &changed );
				if ( changed ){
					radiusChanged = true;
				}
			}
		}
		if ( radiusChanged ){
			_saveStateRegions();
			emit regionsChanged();
		}
	}
	else {
		result = "Minor radius of region must be nonnegative: "+QString::number( radius );
	}
	return result;
}

QString RegionControls::setRegionWidth( double width ){
	QString result;
	if ( width >= 0 ){
		bool widthChanged = false;
		int regionCount = m_regions.size();
		for ( int i = 0; i < regionCount; i++ ){
			if ( m_regions[i]->isSelected() ){
				if ( m_regions[i]->setWidth( width ) ){
					widthChanged = true;
				}
			}
		}
		if ( widthChanged ){
			_saveStateRegions();
			emit regionsChanged();
		}
	}
	else {
		result = "Width of region must be nonnegative: "+QString::number( width );
	}
	return result;
}


void RegionControls::_setRegionsSelected( QStringList ids ){
	int regionCount = m_regions.size();
	int indexCurrent = -1;

	//First set all regions unselected
	for ( int i = 0; i < regionCount; i++ ){
		m_regions[i]->setSelected( false );
	}
	for ( int i = 0; i < regionCount; i++ ){
		QString id = m_regions[i]->getId();
		if ( ids.contains ( id ) ){
			m_regions[i]->setSelected( true );
			if ( indexCurrent == -1 ){
				indexCurrent = i;
			}
		}
	}
	_saveStateRegions();
	if ( indexCurrent >= 0  ){
		setIndexCurrent( indexCurrent );
	}
}

Carta::Lib::VectorGraphics::VGList RegionControls::vgList() const {
	//Get the region graphics
	Carta::Lib::VectorGraphics::VGList vgList;
	int regionCount = m_regions.size();
	Carta::Lib::VectorGraphics::VGComposer comp = Carta::Lib::VectorGraphics::VGComposer( );
	if ( m_regionEdit ){
		Carta::Lib::VectorGraphics::VGList editRegionList = m_regionEdit->getVGList();
		comp.appendList( editRegionList );
	}
	for ( int i = 0; i < regionCount; i++ ){
		Carta::Lib::VectorGraphics::VGList regionList = m_regions[i]->getVGList();
		comp.appendList( regionList );
	}
	return comp.vgList();
}


RegionControls::~RegionControls(){
	if ( m_selectRegion != nullptr ){
		Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
		objMan->destroyObject( m_selectRegion->getId());
		m_selectRegion = nullptr;
	}
}
}
}
