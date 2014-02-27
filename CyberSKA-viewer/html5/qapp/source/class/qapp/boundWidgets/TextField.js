/**
 * TextField that is bound to a global variable.
 *
 * It works just like a regular textfield, but the value is auto-updated whenever the global
 * variable changes.
 *
 * Whether the user edits the textfield, or the value is updated via globalvar, the widget
 * does emit "changeValue" event. To get an event only if the user edited the textfield, use "editValue" event
 */

/* global qx, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)
 */


qx.Class.define("qapp.boundWidgets.TextField",
    {
        extend: qx.ui.form.TextField,

        construct: function( varPath) {
            this.base(arguments, "...");

            this.addListener( "changeValue", function(e) {

                    if( this.m_insideGlobalVarCallback) {
                        return;
                    }

                        var value = e.getData();
                fv.console.log( "tf.chv", value);
//                if( this.m_insideGlobalVarCallback) return;
                this.m_sharedVar.set( value);
                this.fireDataEvent( "editValue", value);
            }, this);

            this.m_insideGlobalVarCallback = true;
            this.m_sharedVar = fv.makeGlobalVariable( varPath, this._sharedVarCB.bind(this), true);
            this.m_insideGlobalVarCallback = false;
            this.m_sharedVar.invokeCallbacks();
        },

        members: {

            m_sharedVar: null,
            m_insideGlobalVarCallback: true,

//            setValue: function ( value) {
//                fv.console.log( "tf.setValue", value);
//                this.base( arguments, value);
//                if( this.m_insideGlobalVarCallback) return;
//                this.m_sharedVar.set( value);
//            },
//
            _sharedVarCB: function( val) {
                this.m_insideGlobalVarCallback = true;
                this.setValue( val);
                this.m_insideGlobalVarCallback = false;
            },

            getSharedVar: function() {
                return this.m_sharedVar;
            }

            /*
            // override - to intercept changeValue events originating as a result of globalvar callback
            fireNonBubblingEvent : function(type, clazz, args)
            {
                fv.console.log( "fnbe ", type);
                // don't fire changeValue events if they originated because of global var change
                if( type === "changeValue" && this.m_insideGlobalVarCallback) {
                    fv.console.log( "  - intercepted");
                    return true;
                }
                else {
//                    return this.base( arguments, type, clazz, args);
                    return arguments.callee.base.apply(this, arguments);
                }
            }
            */

        }

    });

