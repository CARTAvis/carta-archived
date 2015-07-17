/**
 * Displays significant digit user preferences.
 */


/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.CustomUI.SignificantDigits", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
       
        /**
         * Initialization and set-up of the dialog.
         */
        _init: function(){
            this._setLayout( new qx.ui.layout.VBox(2));
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            //Significant Digits
            var digitContainer = new qx.ui.container.Composite();
            digitContainer.setLayout( new qx.ui.layout.VBox(2));
            var signLabel = new qx.ui.basic.Label("Significant Digits:");
            var significantDigits = 6;
            this.m_signSpin = new qx.ui.form.Spinner(1, significantDigits, 10 );
            this.m_signSpin.addListener( skel.widgets.Path.CHANGE_VALUE, this._setSignificantDigits, this );
            digitContainer._add( signLabel );
            digitContainer._add( this.m_signSpin );
            this._add( digitContainer );
            
            this._add( new qx.ui.core.Spacer(), {flex:1});
        },
        
        /**
         * Update the significant digits based on server settings.
         * @param controls {Object} - server-side settings.
         */
        setControls : function( controls ){
            this.m_signSpin.setValue( controls.significantDigits );
        },
        
        /**
         * Sets the server-side id of the object storing significant digits.
         * @param id {String} - server-side id of object handling significant digits.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Set the number of significant digits to use for display purposes.
         * @param digits {Number} a positive integer indicating significant digits.
         */
        _setSignificantDigits : function(){
            if ( this.m_connector !== null ){
                //Notify the server of the new value.
                var digits = this.m_signSpin.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setSignificantDigits";
                var params = "significantDigits:"+digits;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        m_id : null,
        m_connector : null,
        m_signSpin : null
    }

});
