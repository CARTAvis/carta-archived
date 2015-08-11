/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Label widget bound to a state.
 */

/*global qx, mImport */

/**
 @ignore( mImport)
 */

qx.Class.define("skel.boundWidgets.Label", {
    extend : qx.ui.basic.Label,

    /**
     * Constructor
     *
     * @param labelPrefix {String} Text to display at the beginning of the label
     * @param labelPostfix {String|Function} Text to display at the end. Can be function.
     * @param varPath {String} Path to the bound shared variable.
     * @param functionLookup {Function} method of determining the label text from a string.
     *
     * If labelPostfix is a function, it is passed the value, and the value is not
     * rendered.
     */
    construct : function(labelPrefix, labelPostfix, varPath, functionLookup) {
        this.m_connector = mImport("connector");
        this.m_sharedVar = this.m_connector.getSharedVar(varPath);
        this.m_sharedVar.addCB(this._sharedVarCB.bind(this));
        
        this.m_labelPrefix = labelPrefix || "";
        this.m_labelPostfix = labelPostfix || "";
        this.base(arguments, "");
        
        this.m_lookupFunction = functionLookup;
      
        // manually invoke the callback so that the label is immediately updated
        // with the current value
        this._sharedVarCB(this.m_sharedVar.get());
    },

    members : {

        m_sharedVar : null,
        m_lookupFunction : null,
        m_connector : null,
        m_labelPrefix : "",
        m_labelPostfix : "",

        /**
         * Callback for the shared variable. Updates the label with the new value.
         *
         * @param val {String} The new value.
         * @private
         */
        _sharedVarCB : function(val) {
            var labelVal="";
            if ( val ){
                if ( this.m_lookupFunction ){
                    try {
                        var controlObj = JSON.parse( val );
                        labelVal = this.m_lookupFunction( controlObj );
                    }
                    catch( err ) {
                        console.log( "Could not parse: "+val );
                    }
                }
                else {
                    labelVal = val;
                }
            }
            if (typeof this.m_labelPostfix === "function") {
                this.setValue(this.m_labelPrefix + this.m_labelPostfix(labelVal));
            } else {
                this.setValue(this.m_labelPrefix + labelVal + this.m_labelPostfix);
            }
        },
        

        /**
         * Returns a reference to the shared variable this labels is bound to.
         *
         * @return {SharedVar} reference to the shared variable.
         */
        getSharedVar : function() {
            return this.m_sharedVar;
        }

    }

});
