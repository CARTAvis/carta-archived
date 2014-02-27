/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 11/11/13
 * Time: 12:25 AM
 * To change this template use File | Settings | File Templates.
 */

/* global qx, fv */
/* jshint expr: true */

/**
@ignore(fv.*)
*/


qx.Class.define("qapp.boundWidgets.CheckBox",
    {
        extend: qx.ui.form.CheckBox,

        construct: function ( label, varPath) {
            this.m_sharedVar = fv.makeGlobalVariable( varPath, this._sharedVarCB.bind(this), true);
            this.m_suspendApplyValue = true;
            this.base(arguments, label);
            this.m_suspendApplyValue = false;
            this.m_sharedVar.invokeCallbacks();
        },

        members: {

            m_sharedVar: null,
            m_suspendApplyValue: true,

            _applyValue: function ( newValue, oldValue) {
                this.base( arguments, newValue, oldValue);
                if( this.m_suspendApplyValue) return;
                this.m_sharedVar.set( newValue ? "1" : "0");
            },

            _sharedVarCB: function( val) {
                val = val === "1";
                this.setValue( val);
            },

            getSharedVar: function() {
                return this.m_sharedVar;
            }

        }

    });

