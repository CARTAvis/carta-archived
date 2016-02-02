/**
 * Allows the user to customize the display of a profile curve.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsDisplayCurves", {
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
         }
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.groupbox.GroupBox( "Curves", "");
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            overallContainer.setContentPadding(1,1,1,1);
            this._add( overallContainer );
            
            this.m_curveCombo  = new skel.widgets.CustomUI.SelectBox( "setCurveSelected", "name" );
            this.m_curveCombo.setToolTipText( "Select a curve to change its color." );
            overallContainer.add( this.m_curveCombo );
            
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            overallContainer.add( this.m_colorSelector );
        },
        

        
        /**
         * Set the server side id of this plot.
         * @param id {String} the server side id of the object that produced this plot.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_curveCombo.setId( this.m_id );
            
        },
        
        m_id : null,
        m_connector : null,
        m_curveCombo : null,
        m_colorSelector : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
