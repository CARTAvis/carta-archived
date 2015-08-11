/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 11/11/13
 * Time: 12:25 AM
 * To change this template use File | Settings | File Templates.
 */

/* global qx, mImport */
/* jshint expr: true */

/**
 @ignore( mImport)
 */


qx.Class.define( "skel.boundWidgets.Toggle",
    {
        extend: skel.widgets.CustomUI.FancyToggle,

        construct: function( label, varPath)
        {
            
            this.m_connector = mImport( "connector");
            if ( varPath && varPath.length > 0 ){
                this.m_sharedVar = this.m_connector.getSharedVar( varPath);
                this.m_sharedVar.addCB( this._sharedVarCB.bind(this));
            }
            this.m_suspendApplyValue = true;
            this.base( arguments, label );
            this.m_suspendApplyValue = false;
            // manually invoke the callback so that the label is immediately updated
            // with the current value
            if ( this.m_sharedVar ){
                this._sharedVarCB( this.m_sharedVar.get());
            }
        },
        
        events : {
            "toggleChanged" : "qx.event.type.Data"
        },

        members: {

            m_connector        : null,
            m_sharedVar        : null,
            m_suspendApplyValue: true,

            _applyValue: function( newValue, oldValue )
            {
                this.base( arguments, newValue, oldValue );
                if( this.m_suspendApplyValue ) {
                    return;
                }
                if ( this.m_sharedVar ){
                    this.m_sharedVar.set( newValue ? "1" : "0" );
                }
                else {
                    if ( oldValue != newValue ){
                        this.fireDataEvent("toggleChanged", newValue);
                    }
                }
            },

            _sharedVarCB: function( val )
            {
                val = val === "1";
                this.setValue( val );
            },

            getSharedVar: function()
            {
                return this.m_sharedVar;
            }

        }

    } );

