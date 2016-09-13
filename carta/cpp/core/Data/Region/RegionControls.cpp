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
        int regionIndex = _findRegionIndex( regions[i] );
        if ( regionIndex < 0 ){
        	regions[i]->setEditMode( false );
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


void RegionControls::_editDone(){
	if ( m_regionEdit ){
		disconnect( m_regionEdit.get(), & Region::editDone,
		             this, & RegionControls::_editDone );
		m_regionEdit->setEditMode( false );
		m_regionEdit->setActive( true );
		m_regions.push_back( m_regionEdit );
		m_regionEdit = std::shared_ptr<Region>(nullptr);

	}
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

bool RegionControls::_handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev ){
	bool validDrag = false;
	if ( ev.isValid() ){
		validDrag = true;
		if ( m_regionEdit ){
			m_regionEdit->handleDrag( ev );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleDrag( ev );
			}
			if ( regionCount > 0 ){
				emit regionsChanged();
			}
		}
	}
	return validDrag;
}

bool RegionControls::_handleHover( const Carta::Lib::InputEvents::HoverEvent& ev ){
	bool validHover = false;
	if ( ev.isValid() ){
		validHover = true;
		if ( m_regionEdit ){
			m_regionEdit->handleHover( ev.pos() );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleHover( ev.pos() );
			}
			if ( regionCount > 0 ){
				emit regionsChanged();
			}
		}

	}
	return validHover;
}


bool RegionControls::_handleTouch( const Carta::Lib::InputEvents::TouchEvent& ev ){
	bool validTap = false;
	if ( ev.isValid() ){
		validTap = true;
		if ( m_regionEdit ){
			m_regionEdit->handleTouch( ev.pos() );
			emit regionsChanged();
		}
		else {
			int regionCount = m_regions.size();
			for ( int i = 0; i < regionCount; i++ ){
				m_regions[i]->handleTouch( ev.pos() );
			}
			if ( regionCount > 0 ){
				emit regionsChanged();
			}
		}
	}
	return validTap;
}

bool RegionControls::_handleTapDouble( const Carta::Lib::InputEvents::DoubleTapEvent& ev ){
	bool validTap = false;
	if ( ev.isValid() ){
		if ( m_regionEdit ){
			validTap = true;
			m_regionEdit->handleTapDouble( ev.pos() );
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

void RegionControls::_onInputEvent(InputEvent & ev ){
	if ( !_handleHover( Carta::Lib::InputEvents::HoverEvent( ev ) ) ) {
		if ( !_handleTapDouble( Carta::Lib::InputEvents::DoubleTapEvent( ev ) ) ) {
			if ( !_handleTouch( Carta::Lib::InputEvents::TouchEvent( ev ) ) ){
				if( !_handleDrag( Carta::Lib::InputEvents::Drag2Event(ev))) {
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




QString RegionControls::setRegionCreateType( const QString& createType ){
	QString result;
	QString modelType = m_regionTypes->getModelType( createType );
	if ( !modelType.isEmpty() ){

		/**
		 * Todo:  May need to delete or put old region edit on stack when this happens.
		 *
		 */
		//We already have one of the correct type.
		QString actualType = m_regionTypes->getActualType( createType );
		if ( m_regionEdit && m_regionEdit->getRegionType() == actualType ){
			//Maybe set it back to default?
		}
		else {
			//Make a new region to edit.
			m_regionEdit = RegionFactory::makeRegionType( modelType );
			m_regionEdit->setEditMode( true );
			connect( m_regionEdit.get(), SIGNAL(editDone()), this, SLOT(_editDone()));
		}
	}
	else {
		result = "Unrecognized type of region to draw "+createType;
	}
	return result;
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
