/**
 * Controls for setting whether the histogram should be of the whole cube, a single
 * plane, or a range of planes.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramCube", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init( );
    },
    
    statics : {
        CMD_SET_PLANE_MODE : "setPlaneMode",
        CMD_SET_RANGE : "setPlaneRange",
        CMD_SET_PLANE : "setPlaneSingle",
        CHANGE_VALUE : "changeValue"
    },

    members : {

        /**
         * Callback for a server error when setting the plane mode.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorPlaneModeCB :function( anObject ){
            return function( planeMode ){
                if ( planeMode ){
                    anObject.setPlaneMode( planeMode );
                }
            };
        },
        
        /**
         * Callback for a server error when setting the plane range.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorPlaneRangeCB : function( anObject ){
            return function( range ){
                if ( range ){
                    var rangeArray = range.split( ",");
                    var min = parseInt(rangeArray[0]);
                    var max = parseInt( rangeArray[1]);
                    anObject.setPlaneBounds( min, max );
                }
            };
        },
        
        /**
         * Callback for a server error when setting a single plane.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorSinglePlaneCB : function( anObject ){
            return function( plane ){
                if ( plane ){
                    var planeInt = parseInt( plane );
                    anObject.setSelectedPlane( planeInt );
                }
            };
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            this._initPlane();
        },
        
        /**
         * Initialize the controls that determine whether the histogram is of a single
         * plane or multiple planes.
         */
        _initPlane : function(){
            
            var planeContainer = new qx.ui.groupbox.GroupBox("Cube", "");
            planeContainer.setContentPadding(1,1,1,1);
            planeContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( planeContainer );
            
            this.m_planeAll = new qx.ui.form.RadioButton( "All");
            this.m_planeAll.addListener( skel.widgets.Histogram.HistogramCube.CHANGE_VALUE, function(){
                if ( this.m_planeAll.getValue() ){
                    this._planeModeChanged();
                }
            }, this );
            planeContainer.add( this.m_planeAll );
            
            var singleComposite = new qx.ui.container.Composite();
            singleComposite.setLayout(new qx.ui.layout.HBox(2));
            this.m_planeSingle = new qx.ui.form.RadioButton( "Single");
            singleComposite.add( this.m_planeSingle );
            this.m_singlePlaneText = new skel.widgets.CustomUI.NumericTextField( 0, null);
            this.m_singlePlaneText.setIntegerOnly( true );
            this.m_singlePlaneText.addListener( "textChanged", this._sendChangeSinglePlaneCmd, this );
            singleComposite.add( this.m_singlePlaneText );
            this.m_planeSingle.addListener( skel.widgets.Histogram.HistogramCube.CHANGE_VALUE, function(){
                if ( this.m_planeSingle.getValue()){
                    this._planeModeChanged();
                }
            }, this );
            
            var rangeComposite = new qx.ui.container.Composite();
            rangeComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_planeRange = new qx.ui.form.RadioButton( "Range");
            rangeComposite.add( this.m_planeRange );
            this.m_rangeMinSpin = new qx.ui.form.Spinner();
            this.m_rangeMinSpin.setMinimum( 0 );
            this.m_rangeMinSpin.setMaximum( 1000000 );
            this.m_rangeMinSpin.addListener( skel.widgets.Histogram.HistogramCube.CHANGE_VALUE, this._sendChangeBoundsCmd, this);
            rangeComposite.add( this.m_rangeMinSpin );
            var rangeLabel = new qx.ui.basic.Label( "<->");
            rangeComposite.add( rangeLabel );
            this.m_rangeMaxSpin = new qx.ui.form.Spinner();
            this.m_rangeMaxSpin.setMinimum( 0 );
            this.m_rangeMaxSpin.setMaximum( 1000000 );
            this.m_rangeMaxSpin.addListener( skel.widgets.Histogram.HistogramCube.CHANGE_VALUE, this._sendChangeBoundsCmd, this);
            rangeComposite.add( this.m_rangeMaxSpin );
            this.m_planeRange.addListener( skel.widgets.Histogram.HistogramCube.CHANGE_VALUE, function(){
                if ( this.m_planeRange.getValue() ){
                    this._planeModeChanged();
                }
            }, this );
                
            //The value of the max spinner is the maximum value for the min spinner.
            this.m_rangeMaxSpin.bind( "value", this.m_rangeMinSpin, "maximum");
            //The value of the min spinner is the minimum value for the max spinner.
            this.m_rangeMinSpin.bind( "value", this.m_rangeMaxSpin, "minimum");
            
            var radGroup = new qx.ui.form.RadioGroup();
            radGroup.add(this.m_planeAll, this.m_planeSingle, this.m_planeRange );
            planeContainer.add( singleComposite );
            planeContainer.add( rangeComposite );

        },
        
        /**
         * Notify the server that the histogram's plane settings have changed.
         */
        _planeModeChanged : function(){
            var enableSingle = this.m_planeSingle.getValue();
            this.m_singlePlaneText.setEnabled( enableSingle );
            var enableRange = this.m_planeRange.getValue();
            this.m_rangeMinSpin.setEnabled( enableRange );
            this.m_rangeMaxSpin.setEnabled( enableRange );
            if ( this.m_id !== null ){
                var mode = null;
                if ( enableSingle ){
                    mode = this.m_planeSingle.getLabel();
                }
                else if ( enableRange ){
                    mode = this.m_planeRange.getLabel();
                }
                else if ( this.m_planeAll.getValue()){
                    mode = this.m_planeAll.getLabel();
                }
                if ( mode !== null && this.m_connector !== null){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramCube.CMD_SET_PLANE_MODE;
                    var params = "planeMode:"+mode;
                    this.m_connector.sendCommand( cmd, params, this._errorPlaneModeCB( this ));
                }
            }
        },
        
        /**
         * Notify the server that the upper and/or lower bound of planes
         * has changed.
         */
        _sendChangeBoundsCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramCube.CMD_SET_RANGE;
                var params = "planeMin:"+this.m_rangeMinSpin.getValue()+",planeMax:"+this.m_rangeMaxSpin.getValue();
                this.m_connector.sendCommand( cmd, params, this._errorPlaneRangeCB(this));
            }
        },
        
        /**
         * Notify the server that the selected single plane has changed.
         */
        _sendChangeSinglePlaneCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramCube.CMD_SET_PLANE;
                var params = "selectedPlane:"+this.m_singlePlaneText.getValue();
                this.m_connector.sendCommand( cmd, params, this._errorSinglePlaneCB(this));
            }
        },
        
        /**
         * Set a range of planes to use in computing the histogram.
         * @param planeMin {number} a lower (inclusive) bound for the planes.
         * @param planeMax {number} an upper {inclusive} bound for the planes.
         */
        setPlaneBounds : function( planeMin, planeMax ){
            if ( planeMin <= planeMax ){
                if ( this.m_rangeMinSpin.getValue() != planeMin ){
                    this.m_rangeMinSpin.setValue( planeMin );
                }
                if ( this.m_rangeMaxSpin.getValue() != planeMax ){
                    this.m_rangeMaxSpin.setValue( planeMax );
                }
            }
        },
        
        /**
         * Set the index of the single plane for computing the histogram.
         * @param selectedPlane {Number} the index of the plane used to compute the histogram.
         */
        setSelectedPlane : function ( selectedPlane ){
            var plainTextValue = this.m_singlePlaneText.getValue();
            if ( plainTextValue != selectedPlane ){
                this.m_singlePlaneText.setValue( selectedPlane );
            }
        },
        
        /**
         * Set whether or not this histogram is of a single plane, range of planes, or
         *      a full cube.
         * @param mode {String} an identifier for the plane(s) to use in computing the histogram..
         */
        setPlaneMode : function ( mode ){
            if ( mode == this.m_planeAll.getLabel()){
                if ( !this.m_planeAll.getValue()){
                    this.m_planeAll.setValue( true );
                }
            }
            else if ( mode == this.m_planeSingle.getLabel()){
                if ( !this.m_planeSingle.getValue()){
                    this.m_planeSingle.setValue( true );
                }
            }
            else if ( mode == this.m_planeRange.getLabel()){
                if ( !this.m_planeRange.getValue()){
                    this.m_planeRange.setValue( true );
                }
            }
            else {
                console.log( "Unrecognized plane mode: "+mode );
            }
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        m_planeAll : null,
        m_planeSingle : null,
        m_planeRange : null,
        m_singlePlaneText : null,
        m_rangeMinSpin : null,
        m_rangeMaxSpin : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});