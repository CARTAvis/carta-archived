/**
 * Allows the user to customize the display of a profile curve.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsCurves", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Curves", "");
        this.m_colorAppeared = false;
        this._init();
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
             
             var path = skel.widgets.Path.getInstance();
                 
             //Curve styles
             this.m_sharedVar = this.m_connector.getSharedVar(path.LINE_STYLES);
             this.m_sharedVar.addCB(this._lineStylesChangedCB.bind(this));
             this._lineStylesChangedCB();
             
             //Plot styles
             this.m_sharedVarPlot = this.m_connector.getSharedVar(path.PLOT_STYLES);
             this.m_sharedVarPlot.addCB(this._plotStylesChangedCB.bind(this));
             this._plotStylesChangedCB();
         }
    },

    members : {
        
        /**
         * Update the list of profile curves based on server information.
         * @param curveUpdate {Object} - information from the server about profile curves.
         */
        dataUpdate : function( curveUpdate ){
            this.m_curveInfo = curveUpdate.curves;
            var curveNames = [];
            var j = 0;
            for ( var i = 0; i < this.m_curveInfo.length; i++ ){
            	if ( this.m_curveInfo[i].active ){
            		curveNames[j] = this.m_curveInfo[i].name;
            		j++;
            	}
            }
            this.m_curveList.setItems( curveNames );
            this._updateColor();
            this._updateStylePlot();
            this._updateStyle();
        }, 
       

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.container.Composite();
            overallContainer.setLayout( new qx.ui.layout.HBox(1));
            this._add( overallContainer );
            
            var TABLE_WIDTH = 150;
            this.m_curveList  = new skel.widgets.CustomUI.ItemTable( "Profiles", TABLE_WIDTH);
            this.m_curveList.setTestId( "profileTable");
            this.m_curveList.setToolTipText( "Select one or more profiles to customize the display." );
            this.m_curveList.setWidth( TABLE_WIDTH );
            this.m_curveList.addListener( "itemsSelected", this._updateColor, this );
            overallContainer.add( this.m_curveList );
            
            var curveContainer = new qx.ui.container.Composite();
            curveContainer.setLayout( new qx.ui.layout.VBox(1));
            
            var styleContainer = new qx.ui.container.Composite();
            styleContainer.setLayout( new qx.ui.layout.HBox(1));
             var styleLabel = new qx.ui.basic.Label( "Style:");
            this.m_lineCombo = new skel.widgets.CustomUI.SelectBox();
            this.m_lineCombo.setToolTipText( "Select a plot style for the curve.");
            this.m_lineCombo.addListener( "selectChanged", this._sendStylePlotCmd, this );
            this.m_styleCombo = new skel.widgets.CustomUI.SelectBox();
            this.m_styleCombo.setToolTipText( "Select a line style for the curve.");
            this.m_styleCombo.addListener( "selectChanged", this._sendStyleChangeCmd, this );
            styleContainer.add( styleLabel );
            styleContainer.add( this.m_styleCombo );
            styleContainer.add( this.m_lineCombo );
            curveContainer.add( styleContainer );
            
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            this.m_colorSelector.addListener("appear", function() {
                this.m_colorAppeared = true;
                this._updateColor();
            }, this );
            this.m_colorListenerId = this.m_colorSelector.addListener( "changeValue", this._sendColorChangeCmd, this );
            curveContainer.add( this.m_colorSelector );
            
            overallContainer.add( curveContainer );
        },
        
        /**
         * Callback for a change in the available line styles.
         */
        _lineStylesChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var styles = obj.lineStyles;
                        this.m_styleCombo.setSelectItems( styles );
                    }
                    catch( err ){
                        console.log( "Could not parse line styles: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Callback for a change in the list of available plot styles from the
         * server.
         */
        _plotStylesChangedCB : function(){
            if ( this.m_sharedVarPlot ){
                var val = this.m_sharedVarPlot.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var styles = obj.plotStyles;
                        this.m_lineCombo.setSelectItems( styles );
                    }
                    catch( err ){
                        console.log( "Could not parse plot styles: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        
        /**
         * Notify the server that the user has changed the color of a profile curve.
         */
        _sendColorChangeCmd : function(){
            if ( this.m_id !== null ){
                var red = this.m_colorSelector.getRed();
                var green = this.m_colorSelector.getGreen();
                var blue = this.m_colorSelector.getBlue();
                var curves = this.m_curveList.getSelected();
                var nameList = curves.join(";")
                var params = "red:"+red+",green:"+green+",blue:"+blue+",name:"+nameList;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setCurveColor";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Notify the server that the user has changed the color of a profile curve.
         */
        _sendStyleChangeCmd : function(){
            if ( this.m_id !== null ){
                var style = this.m_styleCombo.getValue();
                var curves = this.m_curveList.getSelected();
                var nameList = curves.join(";");
                var params = "style:"+style+",name:"+nameList;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLineStyle";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send the new plot style to the server.
         */
        _sendStylePlotCmd : function(){
            if ( this.m_id !== null ){
                var style = this.m_lineCombo.getValue();
                var curves = this.m_curveList.getSelected();
                var nameList = curves.join(";");
                var params = "style:"+style+",name:"+nameList;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setPlotStyle";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        
        /**
         * Set the server side id of this plot.
         * @param id {String} the server side id of the object that produced this plot.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        
        /**
         * Update the color display when the selected profile curve changes.
         */
        _updateColor : function(){
            if ( this.m_curveList !== null && this.m_colorSelector !== null &&
                    this.m_curveInfo !== null ){
                if ( this.m_colorAppeared ){
                    this.m_colorSelector.removeListenerById( this.m_colorListenerId );
                    var curveIndex = this.m_curveList.getSelectedIndex();
                    if ( curveIndex < this.m_curveInfo.length ){
                        var oldRed = this.m_colorSelector.getRed();
                        var newRed = this.m_curveInfo[curveIndex].red;
                        if ( oldRed != newRed ){
                            this.m_colorSelector.setRed( newRed );
                        }
                        var oldGreen = this.m_colorSelector.getGreen();
                        var newGreen = this.m_curveInfo[curveIndex].green;
                        if ( oldGreen != newGreen ){
                            this.m_colorSelector.setGreen( newGreen );
                        }
                        var oldBlue = this.m_colorSelector.getBlue();
                        var newBlue = this.m_curveInfo[curveIndex].blue;
                        if ( oldBlue != newBlue ){
                            this.m_colorSelector.setBlue( newBlue );
                        }
                    }
                    this.m_colorListenerId = this.m_colorSelector.addListener( "changeValue", 
                            this._sendColorChangeCmd, this );
                }
            }
        },
        
        /**
         * Update the style when the user selects a new profile curve.
         */
        _updateStyle : function(){
            if ( this.m_curveList !== null && this.m_curveInfo !== null ){
                var curveIndex = this.m_curveList.getSelectedIndex();
                if ( curveIndex < this.m_curveInfo.length ){
                    var style = this.m_curveInfo[curveIndex].lineStyle;
                    this.m_styleCombo.setSelectValue( style );
                }
            }
        },
        
        _updateStylePlot : function(){
            if ( this.m_curveList !== null && this.m_curveInfo !== null ){
                var curveIndex = this.m_curveList.getSelectedIndex();
                if ( curveIndex < this.m_curveInfo.length ){
                    var style = this.m_curveInfo[curveIndex].plotStyle;
                    this.m_lineCombo.setSelectValue( style );
                }
            }
        },
        
        m_id : null,
        m_colorAppeared : null,
        m_connector : null,
        m_curveList : null,
        m_curveInfo : null,
        m_colorSelector : null,
        m_colorListenerId : null,
        m_lineCombo : null,
        m_sharedVar : null,
        m_sharedVarPlot : null,
        m_styleCombo : null
    }
});
