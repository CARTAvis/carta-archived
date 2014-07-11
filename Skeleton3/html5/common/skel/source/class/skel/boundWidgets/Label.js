/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Label widget bound to a state.
 */

/*global qx, mImport */

/**
 @ignore( mImport)
 */


qx.Class.define("skel.boundWidgets.Label",
    {
        extend: qx.ui.basic.Label,

        /**
         * Constructor
         * @param labelPrefix {String}
         * @param labelPostfix {String|Function}
         * @param varPath {String} Path to the bound shared variable
         *
         * If postfix is a function, it is passed the value as well.
         */
        construct: function ( labelPrefix, labelPostfix, varPath) {
            this.m_connector = mImport( "connector");
            this.m_sharedVar = this.m_connector.getSharedVar( varPath);
         
            this.m_sharedVar.addCB(this._sharedVarCB.bind(this));
            this.m_labelPrefix = labelPrefix || "";
            this.m_labelPostfix = labelPostfix || "";
            this.base(arguments, "");

            // manually invoke the callback so that the label is immediately updated
            // with the current value
            this._sharedVarCB( this.m_sharedVar.get());
        },

        members: {

            m_sharedVar: null,
            m_connector: null,
            m_labelPrefix: "",
            m_labelPostfix: "",

//            _applyValue: function ( newValue, oldValue) {
//                fv.console.log( "_applyValue-" + this.getLabel(), newValue, oldValue);
//                this.m_sharedVar.set( newValue ? "1" : "0");
//                this.base( arguments, newValue, oldValue);
//            },

            _sharedVarCB: function( val) {
                if( typeof this.m_labelPostfix === "function") {
                    this.setValue( this.m_labelPrefix + this.m_labelPostfix( val));
                }
                else {
                    this.setValue( this.m_labelPrefix + val + this.m_labelPostfix);
                }
            },

            getSharedVar: function() {
                return this.m_sharedVar;
            }

        }

    });

