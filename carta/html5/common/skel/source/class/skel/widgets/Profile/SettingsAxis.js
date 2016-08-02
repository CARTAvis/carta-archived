/**
 * Allows the user to set the units for the axes of the profile plot..
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsAxis", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init();
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
             
             var path = skel.widgets.Path.getInstance();
             
             //Spectral units
             this.m_sharedVarUnitBottom = this.m_connector.getSharedVar(path.SPECTRAL_UNITS);
             this.m_sharedVarUnitBottom.addCB(this._unitsBottomChangedCB.bind(this));
             this._unitsBottomChangedCB();
             
             //Intensity units
             this.m_sharedVarUnitLeft = this.m_connector.getSharedVar(path.INTENSITY_UNITS);
             this.m_sharedVarUnitLeft.addCB(this._unitsLeftChangedCB.bind(this));
             this._unitsLeftChangedCB();
         }
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.groupbox.GroupBox( "Axes", "");
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            overallContainer.setContentPadding(1,1,1,1);
            this._add( overallContainer );
            
            var unitStr = "units";
            this.m_axisBottom  = new skel.widgets.CustomUI.SelectBox( "setAxisUnitsBottom", unitStr );
            this.m_axisBottom.setToolTipText( "Select units for the x-axis." );
            var axisBottomLabel = new qx.ui.basic.Label( "X Axis:");
            this.m_axisLeft = new skel.widgets.CustomUI.SelectBox( "setAxisUnitsLeft", unitStr );
            this.m_axisLeft.setToolTipText( "Select units for the y-axis." );
            var axisLeftLabel = new qx.ui.basic.Label( "Y Axis:");
            var gridLayout = new qx.ui.layout.Grid();
            overallContainer.setLayout( gridLayout );
            overallContainer.add( axisLeftLabel, {row:0,column:0});
            overallContainer.add( this.m_axisLeft, {row:1, column:0});
            overallContainer.add( axisBottomLabel, {row:2, column:0});
            overallContainer.add( this.m_axisBottom, {row:3,column:0});
        },
        
        /**
         * Set the bottom axis units.
         * @param unitStr {String} - the axis units to be displayed.
         */
        setAxisBottomUnits : function( unitStr ){
            this.m_axisBottom.setSelectValue( unitStr, false );
        },
        
        /**
         * Set the left axis units.
         * @param unitStr {String} - the axis units to be displayed.
         */
        setAxisLeftUnits : function( unitStr ){
            this.m_axisLeft.setSelectValue( unitStr, false );
        },
        
        /**
         * Set the server side id of this plot.
         * @param id {String} the server side id of the object that produced this plot.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_axisBottom.setId( id );
            this.m_axisLeft.setId( id );
        },
        
        /**
         * Updates the channel units change on the server.
         */
        _unitsBottomChangedCB : function(){
            if ( this.m_sharedVarUnitBottom ){
                var val = this.m_sharedVarUnitBottom.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var units = obj.units;
                        this.m_axisBottom.setSelectItems( units );
                    }
                    catch( err ){
                        console.log( "Could not parse bottom intensity units: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Updates the channel units change on the server.
         */
        _unitsLeftChangedCB : function(){
            if ( this.m_sharedVarUnitLeft ){
                var val = this.m_sharedVarUnitLeft.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var units = obj.units;
                        this.m_axisLeft.setSelectItems( units );
                    }
                    catch( err ){
                        console.log( "Could not parse left intensity units: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        m_axisBottom : null,
        m_axisLeft : null,
        m_id : null,
        m_connector : null,
        m_sharedVarUnitBottom : null,
        m_sharedVarUnitLeft : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
