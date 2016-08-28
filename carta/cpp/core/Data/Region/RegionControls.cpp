#include "RegionControls.h"
#include "Region.h"
#include "RegionTypes.h"
#include "Data/Util.h"
#include "Data/Selection.h"
//#include "Shape/ControlPoint.h"
//#include "Shape/ShapeController.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"

#include <QDebug>
//#include <QString>

namespace Carta {

namespace Data {

const QString RegionControls::CLASS_NAME = "RegionControls";
const QString RegionControls::REGIONS = "regions";


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
 	 m_selectRegion( nullptr ){

	m_regionCreateType = m_regionTypes->getDefault();

	_initializeStatics();
	_initializeSelections();
	_initializeCallbacks();
	_initializeState();
}

void RegionControls::_addDataRegions( std::vector<std::shared_ptr<Region>> regions ){
    int count = regions.size();
    for ( int i = 0; i < count; i++ ){
        int regionIndex = _findRegionIndex( regions[i] );
        if ( regionIndex < 0 ){
            m_regions.push_back( regions[i]);
        }
    }
    count = m_regions.size();
    m_selectRegion->setUpperBound( count );
    //The last loaded region should be selected.
    m_selectRegion->setIndex( count - 1 );
    _saveStateRegions();
    emit regionsChanged();
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
        //m_regions.removeAt( index );
        m_regions.erase( m_regions.begin() + index );
        regionRemoved = true;

    }
    if ( regionRemoved ){
        _saveStateRegions();
        emit regionsChanged();
    }
    else {
        result = "Could not find region to remove for index="+QString::number(index);
    }
    return result;
}


int RegionControls::_findRegionIndex( std::shared_ptr<Region> region ) const {
    int index = -1;
    if ( region ){
        QJsonObject info = region->toJSON();
        int regionCount = m_regions.size();
        for ( int i = 0; i < regionCount; i++ ){
            QJsonObject otherInfo = m_regions[i]->toJSON();
            if ( info == otherInfo ){
                index = i;
                break;
            }
        }
    }
    return index;
}

std::shared_ptr<Region> RegionControls::getRegion() const {
    int regionIndex = m_selectRegion->getIndex();
    std::shared_ptr<Region> region( nullptr );
    int regionCount = m_regions.size();
    if ( regionIndex >= 0 && regionIndex < regionCount ){
        region = m_regions[regionIndex];
    }
    return region;
}

std::vector<std::shared_ptr<Region> > RegionControls::getRegions() const {
    return m_regions;
}


int RegionControls::getSelectRegionIndex() const {
    int selectRegionIndex = -1;
    if ( m_regions.size() > 0 ){
        selectRegionIndex = m_selectRegion->getIndex();
    }
    return selectRegionIndex;
}

void RegionControls::_initializeCallbacks(){
    addCommandCallback( "shapeChanged", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { "info"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        //resetStateData( dataValues[ *keys.begin()]);
        return "";
    });

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
           return result;
       });
}

void RegionControls::_initializeSelections(){
	Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
	//connect( m_selectImage, SIGNAL(indexChanged()), this, SIGNAL(viewLoad()));
	m_selectRegion = objMan->createObject<Selection>();
}

void RegionControls::_initializeState(){
    int regionCount = m_regions.size();
    m_state.insertArray(REGIONS, regionCount );
    m_state.flushState();
}

void RegionControls::_initializeStatics(){
    if ( m_regionTypes == nullptr ){
        m_regionTypes = Util::findSingletonObject<RegionTypes>();
    }
}

void RegionControls::_onInputEvent(const InputEvent & ev ){
	if ( !_handleHover( Carta::Lib::InputEvents::HoverEvent( ev ) ) ) {
		if ( !_handleTouch( Carta::Lib::InputEvents::TouchEvent( ev ) ) ) {
			if ( !_handleTap( Carta::Lib::InputEvents::DoubleTapEvent( ev ) ) ) {
				if( !_handleDrag( Carta::Lib::InputEvents::Drag2Event(ev))) {
					//qWarning() << "RegionControls:: unhandled event: "<<ev.type();
				}
			}
		}
	}
}

bool RegionControls::_handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev ){
	bool validDrag = false;
	if ( ev.isValid() ){

	}
	return validDrag;
}

bool RegionControls::_handleHover( const Carta::Lib::InputEvents::HoverEvent& ev ){
	bool validHover = false;
	if ( ev.isValid() ){

	}
	return validHover;
}

bool RegionControls::_handleTap( const Carta::Lib::InputEvents::DoubleTapEvent& ev ){
	bool validTap = false;
	if ( ev.isValid() ){

	}
	return validTap;
}

bool RegionControls::_handleTouch( const Carta::Lib::InputEvents::TouchEvent& ev ){
	bool validTouch = false;
	if ( ev.isValid() ){

	}
	return validTouch;
}


void RegionControls::_saveStateRegions(){
    //Regions
    int regionCount = m_regions.size();
    int oldRegionCount = m_state.getArraySize( REGIONS);
    if ( regionCount != oldRegionCount){
        m_state.resizeArray( REGIONS, regionCount, Carta::State::StateInterface::PreserveNone );
    }
    for ( int i = 0; i < regionCount; i++ ){
        QString regionKey = Carta::State::UtilState::getLookup( REGIONS, i);
        QString regionTypeStr= m_regions[i]->_getStateString();
        m_state.setObject( regionKey, regionTypeStr );
    }
    m_state.flushState();
}



QString RegionControls::_setRegionCreateType( const QString& createType ){
	QString result;
	QString actualType;
	bool valid = m_regionTypes->isRegionType( createType, actualType );
	if ( valid ){
		m_regionCreateType = actualType;
	}
	else {
		result = "Unrecognized region type: " + createType;
	}
	return result;
}


Carta::Lib::VectorGraphics::VGList RegionControls::vgList() const {
	//Get the region graphics
	Carta::Lib::VectorGraphics::VGList vgList;
	int regionCount = m_regions.size();
	Carta::Lib::VectorGraphics::VGComposer comp = Carta::Lib::VectorGraphics::VGComposer( );
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
