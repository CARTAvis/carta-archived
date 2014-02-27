/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 11/11/13
 * Time: 8:03 PM
 *
 * To change this template use File | Settings | File Templates.
 */

/* global qx, qapp, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)
 */



qx.Class.define("qapp.boundWidgets.Slider",
    {
        extend: qx.ui.form.Slider,

        construct: function ( settings) {
            settings = settings || {};
            var orientation = settings.orientation || "horizontal";
            var path = settings.path;
            fv.assert( path != null);
            this.m_min = settings.min || 0;
            this.m_max = settings.max || 10;
            this.m_sharedVar = fv.makeGlobalVariable( path, this._sharedVarCB.bind(this), true);
            this.m_suspendApplyValue = true;
            this.base(arguments, orientation);
            this.set({ minimum: 0, maximum: 1000, pageStep: 100, singleStep: 1 });
            this.m_suspendApplyValue = false;
            this.m_sharedVar.invokeCallbacks();
        },

        members: {

            m_sharedVar: null,
            m_suspendApplyValue: true,

            _applyValue: function ( newValue, oldValue) {
                fv.console.log( "_applyValue-slider", newValue, oldValue);
                this.base( arguments, newValue, oldValue);
                // if we are in suspend mode, we are done
                if( this.m_suspendApplyValue) return;
                // we need to update shared variable
                // first convert the newValue to a proper float
                var floatVal = fv.lib.linMap( newValue, this.getMinimum(), this.getMaximum(),
                    this.m_min, this.m_max
                );
                if( isFinite( floatVal)) {
                    floatVal = floatVal.toString();
                }
                else {
                    floatVal = "nan";
                }
                // now set the shared variable
                this.m_sharedVar.set( floatVal);
            },

            _sharedVarCB: function( val) {
                fv.console.log( "_sharedVarCB:", val);
                // convert the incoming string to integer suitable for the slider
                var intVal = parseFloat( val);
                intVal = fv.lib.linMap( intVal, this.m_min, this.m_max, this.getMinimum(), this.getMaximum());
                if( ! isFinite( intVal)) intVal = this.getMinimum();
                intVal = Math.round( intVal);
                intVal = fv.lib.clamp( intVal, this.getMinimum(), this.getMaximum());
                // apply the integer value to the slider, but don't update the state
                this.m_suspendApplyValue = true;
                this.setValue( intVal);
                this.m_suspendApplyValue = false;
            },

            getSharedVar: function() {
                return this.m_sharedVar;
            }

        }

    });

