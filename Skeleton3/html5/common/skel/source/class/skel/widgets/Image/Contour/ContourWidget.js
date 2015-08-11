/**
 * Displays controls for customizing a particular contour level.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Contour.ContourWidget", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init( );
        
        //Shared variable for method used to generate contour levels.
        this.m_connector = mImport( "connector" );
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarLineStyles = this.m_connector.getSharedVar(pathDict.CONTOUR_LINE_STYLES);
        this.m_sharedVarLineStyles.addCB(this._lineStylesChangedCB.bind(this));
        this._lineStylesChangedCB();
    },
    

    members : {
        

        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.VBox(2));
            
            this.m_thicknessWidget = new skel.widgets.CustomUI.TextSlider("setThickness", "thickness",
                    0,10, 2, "Thickness", true, "Set the thickness of the contours.", 
                    "Slide to set the thickness of the contours.",
                    "contourThicknessField", "contourThicknessSlider", false);
            this.m_alphaWidget = new skel.widgets.CustomUI.TextSlider("setAlpha", "alpha",
                    0,255, 128, "Alpha", true, "Set the transparency of the contours.", 
                    "Slide to set the transparency of the contours.",
                    "contourAlphaField", "contourAlphaSlider", false);
            
            var lineStyleLabel = new qx.ui.basic.Label( "Style:");
            this.m_lineStyleCombo = new skel.boundWidgets.ComboBox();
            this.m_lineStyleCombo.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendStyle, this );
            
            this.m_visibleCheck = new qx.ui.form.CheckBox();
            var visibleLabel = new qx.ui.basic.Label( "Show ");
            
            var visibleContainer = new qx.ui.container.Composite();
            visibleContainer.setLayout( new qx.ui.layout.HBox(2));
            visibleContainer.add( visibleLabel);
            visibleContainer.add( this.m_visibleCheck);
            
            var lineContainer = new qx.ui.container.Composite();
            lineContainer.setLayout( new qx.ui.layout.HBox(2));
            lineContainer.add( lineStyleLabel );
            lineContainer.add( this.m_lineStyleCombo );
            
            var settingsContainer = new qx.ui.container.Composite();
            settingsContainer.setLayout( new qx.ui.layout.Grid());
            settingsContainer.add( visibleContainer, {row:0,column:0});
            settingsContainer.add( lineContainer, {row:0,column:1});
            settingsContainer.add( this.m_thicknessWidget, {row:1,column:0} );
            settingsContainer.add( this.m_alphaWidget, {row:1,column:1} );
            
            
            //Color
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            this.m_colorSelector.addListener( 'appear', function(){
                this.m_colorAppeared = true;
                this._setColor( this.m_red, this.m_green, this.m_blue);
            }, this );
            
            this._add( settingsContainer );
            this._add( this.m_colorSelector );
        },
        
        /**
         * Callback for a change in the available line styles from the server.
         */
        _lineStylesChangedCB : function(){
            if ( this.m_sharedVarLineStyles ){
                var val = this.m_sharedVarLineStyles.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_lineStyleCombo.setComboItems( obj.lineStyles );
                    }
                    catch( err ){
                        console.log( "Contour Set page could not parse line styles: "+val );
                        console.log( "Error: "+err );
                    }
                }
            }
        },
        
        
        /**
         * Update the UI with new contour information.
         * @param contour {Object} - detailed information about the contour.
         */
        setContour : function( contour ){
            this.m_level = contour.level;
            this._setVisible( contour.visible );
            this._setWidth( contour.width );
            this._setStyle( contour.style );
            
            this._setColor( contour.red, contour.blue, contour.green);
            this._setTransparency( contour.alpha );
            this.m_level = contour.level;
        },
        
        /**
         * Send a command to the server with an update on the line style to use for
         * this contour.
         */
        _sendStyle : function(){
            if ( this.m_connector !== null ){
                var style = this.m_styleCombo.getTextSelection();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND +"setStyle";
                var params = "style:"+style+",id:"+this.m_id+",level:"+this.m_level+",set:"+this.m_contourSetName;
                console.log( "Sending params="+params);
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Set the color of the contour based on information from the server.
         * @param red {Number} - the amount of red.
         * @param green {Number} - the amount of green.
         * @param blue {Number} - the amount of blue.
         */
        _setColor : function( red, green, blue ){
            if ( this.m_colorAppeared ){
                this.m_colorSelector.setRed( red );
                this.m_colorSelector.setGreen( green );
                this.m_colorSelector.setBlue( blue );
            }
            else {
                this.m_red = red;
                this.m_green = green;
                this.m_blue = blue;
            }
        },
        
        /**
         * Update the drawing style for this contour based on server values.
         * @param style {String} - the drawing style for the contour.
         */
        _setStyle : function( style ){
            if ( style != this.m_lineStyleCombo.getValue() ){
                this.m_lineStyleCombo.setComboValue( style );
            }
        },
        
        /**
         * Set the transparency of the contour based on server values.
         * @param alpha {Number} - the transparency level of the contour.
         */
        _setTransparency : function( alpha ){
            if ( this.m_alphaWidget.getValue() != alpha ){
                this.m_alphaWidget.setValue( alpha );
            }
        },
        
        /**
         * Set whether or not the contour should be visible.
         * @param visible {boolean} - true if the contour should be visible;
         *      false otherwise.
         */
        _setVisible : function( visible ){
            if ( this.m_visibleCheck.getValue() != visible ){
                this.m_visibleCheck.setValue( visible );
            }
        },
        
        /**
         * Set the thickness of the contour.
         * @param width {Number} - thickness of the pen used to draw the contour.
         */
        _setWidth : function( width ){
            if ( this.m_thicknessWidget.getValue() != width ){
                this.m_thicknessWidget.setValue( width );
            }
        },
        
        /**
         * Set the server side id of the contour control object.
         * @param id {String} - the server-side id of the contour control object.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Set the name of the contour set that this Contour is a member of.
         * @param name {String} - the unique name of the parent contour set.
         */
        setContourSetName : function( name ){
            this.m_contourSetName = name;
        },
        
        //Identification
        m_id : null,
        m_contourSetName : null,
        m_level : null,
        
        m_connector : null,
        m_sharedVarLineStyles : null,
       
        m_lineStyleCombo : null,
        m_thicknessWidget : null,
        m_alphaWidget : null,
        m_colorSelector : null,
        m_visibleCheck : null,
        
        m_colorAppeared : false,
        m_red : null,
        m_green : null,
        m_blue : null
    }
});