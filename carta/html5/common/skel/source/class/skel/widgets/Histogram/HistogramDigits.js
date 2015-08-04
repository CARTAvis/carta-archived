/**
 * UI for displaying histogram significant digits.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramDigits", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },


    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            var displayContainer = new qx.ui.groupbox.GroupBox("Precision", "");
            displayContainer.setContentPadding(1,1,1,1);
            displayContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( displayContainer );
            this.m_digitSettings = new skel.widgets.CustomUI.SignificantDigits();
            displayContainer.add( this.m_digitSettings );
        },
        
        /**
         * Updates the significant digits with a server-side value.
         * @param controls {String} - server-side information.
         */
        setControls : function( controls ){
            this.m_digitSettings.setControls( controls );
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_digitSettings.setId( id );
        },
        
        m_digitSettings : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});