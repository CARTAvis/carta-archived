/**
 * TextField bound to a state..
 */

/* global qx, mImport */
/* jshint expr: true */

/**
 @ignore( mImport)
 */


qx.Class.define( "skel.boundWidgets.TextField", {

    extend: qx.ui.form.TextField,

    construct: function( varPath )
    {
        this.base( arguments, "..." );

        // create shared var and register callback
        this.m_connector = mImport( "connector" );
        this.m_sharedVar = this.m_connector.getSharedVar( varPath );
        this.m_sharedVar.addCB( this._sharedVarCB.bind( this ) );

        // manually invoke the callback so that the text field is immediately updated
        // with the current value
        this._sharedVarCB( this.m_sharedVar.get() );

        // listen to changed text
        this.addListener( "changeValue", this._changeValueCB.bind( this ) );
    },

    events: {
        //"toggleChanged" : "qx.event.type.Data"
    },

    members: {

        m_connector        : null,
        m_sharedVar        : null,
        m_insideSharedVarCB: false,

        // changed text callback. Does nothing if called as a result of shared var
        // callback to avoid infinite recursion (or at least unnecessary work)
        _changeValueCB     : function( e )
        {
            if( this.m_insideSharedVarCB ) {
                return;
            }
            var value = e.getData();
            this.m_sharedVar.set( value );
        },

        _sharedVarCB: function( val )
        {
            this.m_insideSharedVarCB = true;
            this.setValue( val );
            this.m_insideSharedVarCB = false;
        }
    }

} );

