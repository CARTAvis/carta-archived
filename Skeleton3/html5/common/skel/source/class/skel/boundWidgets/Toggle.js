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
        extend: skel.widgets.FancyToggle,

        construct: function( label, varPath )
        {
            this.m_connector = mImport( "connector");
            this.m_sharedVar = this.m_connector.getSharedVar( varPath);
            this.m_sharedVar.addCB( this._sharedVarCB.bind(this));
            this.m_suspendApplyValue = true;
            this.base( arguments, label );
            this.m_suspendApplyValue = false;
            // manually invoke the callback so that the label is immediately updated
            // with the current value
            this._sharedVarCB( this.m_sharedVar.get());
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
                this.m_sharedVar.set( newValue ? "1" : "0" );
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

