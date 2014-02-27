/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 11/11/13
 * Time: 7:04 PM
 *
 * To change this template use File | Settings | File Templates.
 */

/* global qx, qapp, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)
 */


qx.Class.define("qapp.boundWidgets.Label",
    {
        extend: qx.ui.basic.Label,

        /**
         * Constructor
         * @param labelPrefix {String}
         * @param labelPostfix {String|Function}
         * @param varPath {String} Path to the bound shared variable
         */
        construct: function ( labelPrefix, labelPostfix, varPath) {
            this.m_sharedVar = fv.makeGlobalVariable( varPath, this._sharedVarCB.bind(this), true);
            this.m_labelPrefix = labelPrefix || "";
            this.m_labelPostfix = labelPostfix || "";
            this.base(arguments, "");
            this.m_sharedVar.invokeCallbacks();
        },

        members: {

            m_sharedVar: null,
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

