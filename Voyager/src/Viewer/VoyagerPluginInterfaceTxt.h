#ifndef VOYAGERPLUGININTERFACE_H
#define VOYAGERPLUGININTERFACE_H

#include <QString>
#include <QtPlugin>

#include <vector>

/**
 * Communication from the core to the plug-ins.
 */
//Note: Investigate Why regions are part of the image interface.

class NGImageI
{
public:
    virtual ~NGImageI() {};

    //Returns a Voyager Unit.
    virtual const Unit& getUnit( ) const = 0;


    virtual std::vector<int> dims()= 0;

    //Used to determine the type of the image (raster, contour, vector).
    //May need to evolve.
    virtual QString imageType() const = 0;

    //Textual representation of the image.
    virtual QString name( bool fullName ) const = 0;

    // Has the object really a mask?
     virtual Bool isMasked() const;

     /**
      * @param which is an enum indicating whether the slice is wanted, the
      *   mask is wanted, or both are wanted.
      */
     virtual void getSlice( const Slicer& slicer, int which,
     		NDArray& slice, NDBoolArray& sliceMask ) = 0;

     // The ImageInfo object contains some miscellaneous information about the image
      // which unlike that stored in MiscInfo, has a standard list of things,
      // such as the restoring beam.
      //
      // Note that setImageInfo REPLACES the information with the new information.
      // It is up to the derived class to make the ImageInfo permanent.
      // <group>
      virtual const ImageInfo& imageInfo() const = 0
      // </group>


};

class ImageInfo {
	//Beam related information
	//Miscellaneous records.
};

/**
 *
 */
/*class IVoyImageReader : public IVoyImage
{
public:


};*/

/*
 * Note:  order of initialization of plug-ins needs to be considered.
 * For example, new image plug-ins might need to be loaded before
 * algorithm plug-ins.  Dependencies may need to be considered.
 */
class NGPluginI
{
public:
    virtual ~NGPluginI() {}

    //What the plug-in does.
    virtual QString description( bool longDescription ) = 0;


    //Identifier to be used to refer to the plug-in.  Must be unique.
    //In the format edu.nrao.pluginName
    virtual QString UID() = 0;

    //Return a list of event identifiers that this plugin is interested in
    //being notified about.
    virtual QList<int> getEvents() = 0;

    //An event has occurred that the plug-in should respond to.
    virtual void handlePlugInEvent( PlugInEvent* ev );



};

/**
 * Interface for calculating a numerical result from a region in an image.
 */

class AlgorithmImageRegionI {
public:
	AlgorithmImageRegionI();
	/**
	 * XML that can be used to generate a GUI where the user can enter inputs
	 * for the algorithm.  Possibly using rappture format.
	 */
	virtual QString getXMLView() const = 0;
	/**
	 * @param image the input image.
	 * @param imageRegion the region of interest in the region.
	 * @param record a Record representing addition inputs to the algorithm (possibly empty if all
	 * 			parameters are default).
	 */
	virtual void compute( const NGImage& image, const ImageRegion& imageRegion, const Record& record ) = 0;
	/**
	 * Return the results of the algorithm as a record.
	 */
	virtual Record getResultAsRecord() const = 0;
	/**
	 * Return the results of the algorithm as text.
	 */
	virtual QString getResultAsText() const = 0;
	/**
	 * Return the results of the algorithm as html.
	 */
	virtual QString getResultAsHtml() const = 0;
	~AlgorithmImageRegionI();

};

class AlgorithmImageRegionDefault : public AlgorithmImageRegionI, public NGImageI {
public:
	AlgorithmImageRegionDefault();
	/**
	 * XML that can be used to generate a GUI where the user can enter inputs
	 * for the algorithm.  Possibly using rappture format.
	 */
	virtual QString getXMLView() const {
		return "";
	}
	/**
	 * @param image the input image.
	 * @param imageRegion the region of interest in the region.
	 * @param record a Record representing addition inputs to the algorithm (possibly empty if all
	 * 			parameters are default).
	 * @return return the results of the computation as a record.
	 */
	virtual Record doCompute( const NGImage& image, const ImageRegion& imageRegion, const Record& record ) = 0;

	virtual void compute( const NGImage& image, const ImageRegion& imageRegion, const Record& record ){
		m_results = doCompute( image, imageRegion, record );
	}


	/**
	 * Return the results of the algorithm as a record.
	 */
	virtual Record getResultAsRecord() const {
		return m_results;
	}
	/**
	 * Return the results of the algorithm as text.
	 */
	virtual QString getResultAsText() const {
		Record record = getResultAsRecord();
		return record.toString(); //Or similar
		//Iterate through fields of record and return them as (key,value) pairs.
	}
	/**
	 * Return the results of the algorithm as html.
	 */
	virtual QString getResultAsHtml() const {
		Record record = getResultAsRecord();
		return Util.toHtml( record );
	}

	virtual QList<int> getEvents(){
		//Note this enum is defined in the core somewhere and the plugins
		//must use them.
		QList<int> eventList(1, RegionSelectionEvent );
		return eventList;
	}

	virtual void handlePlugInEvent( PlugInEvent* ev ){
		PlugInRegionSelectionEvent* selectEv = qobject_cast<PlugInRegionSelectionEvent*>( ev );
		if ( selectEv == nullptr ){
			throw new UnsupportedEvent();
		}
		else {
			const NGImageI& image = selectEv->getImage();
			const ImageRegion& region = selectEv->getRegion();
			Record inputs;
			Record result = doCcompute( image, region inputs );
			selectEvent->setResult( result );
			selectEvent->setHandled( true );
		}

	}

	~AlgorithmImageRegionDefault();
private:
	Record m_results;
};


/**
 * Implementation of the image statistics algorithm as a plugin
 */
class ImageStatistics : public AlgorithmImageRegionDefault{
public:
	ImageStatistics();

	//This example just does the mean.
	virtual Record doCompute( const NGImage& image, const ImageRegion& imageRegion, const Record& record ){
		//m_meanValue = result of computation.
		Record result;
		result.setField( "Mean", meanValue );
		return result;
	}

	//Methods from NGPluginI
	virtual QString description( bool longDescription ){
		QString result = "Computes mean of image region.";
		if ( longDescription ){
			result.append( "\n  The algorithm requires no input parameters.");
		}
		return result;
	}


	virtual QString UID() {
		return "edu.nrao.ImageStatistics";
	}




private:
	double m_meanValue;
};




Q_DECLARE_INTERFACE( INGImage, "org.ngviewer.INGImage" )
Q_DECLARE_INTERFACE( NGPluginInterface, "org.ngviewer.plugininterface" )

#endif // VOYAGERPLUGININTERFACE_H
