/**
 * Container for commands that determine set user preferences.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Preferences.CommandPreferences", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Preferences" );
        this.m_global = true;
        this.m_enabled = true;
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Preferences.Show.CommandShow.getInstance();
        var path = skel.widgets.Path.getInstance();
        var connector = mImport( "connector" );
        this.m_sharedVar = connector.getSharedVar( path.PREFERENCES );
        this.m_sharedVar.addCB(this._preferencesCB.bind(this));
        this._preferencesCB();
    },
    
    members : {
        
        /**
         * Add preference commands that apply only to the selected window.
         * @param cmds {Array} a list of additional window specific preference commands.
         */
        addWindowSpecific : function( cmds ){
            this.m_cmds.push.apply( this.m_cmds, cmds );
        },
        
        /**
         * Remove window specific preference commands.
         */
        clearWindowSpecific : function(){
            var cmdCount = this.m_cmds.length;
            this.m_cmds.splice(1, cmdCount-1);
        },
        
        /**
         * Callback for when user preferences change.
         */
        _preferencesCB : function(){
            var prefVal = this.m_sharedVar.get();
            if ( prefVal ){
                try {
                    var prefObj = JSON.parse( prefVal );
                    for ( var i = 0; i < this.m_cmds.length; i++ ){
                        this.m_cmds[i].setValue( prefObj );
                    }
                }
                catch( err ){
                    console.log( "Could not parse preferences: "+prefVal );
                }
            }
        },
        
        m_sharedVar : null
    }
});