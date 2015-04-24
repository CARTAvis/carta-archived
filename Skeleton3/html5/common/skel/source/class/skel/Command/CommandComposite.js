/**
 * Base class for commands that serve as containers for other commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.CommandComposite", {
    extend : skel.Command.Command,
    
    /**
     * Constructor
     */
    construct : function( label ) {
        this.base( arguments, label, null );
        this.addListener( "changeEnabled", function(){
                if ( ! this.isGlobal() && this.m_cmds !== null ){
                    var enabled = this.getEnabled();
                    for ( var i = 0; i < this.m_cmds.length; i++ ){
                        this.m_cmds[i].setEnabled( enabled );
                    }
                }
            }, this );
    },
    
    members : {
        
        /**
         * Returns the command corresponding to the name if it matches the name of this
         * command or one of the children of this command; otherwise, returns null.
         * @param cmdName {String} the name of a command to look for.
         * @return {skel.Command.Command} the corresponding command or null if no matching command exists in the
         *       children of this object.
         */
        getCommand : function( cmdName ){
            var cmd = arguments.callee.base.apply(this, arguments, cmdName );
            if ( cmd === null && this.m_cmds !== null ){
                for ( var i = 0; i < this.m_cmds.length; i++ ){
                    cmd = this.m_cmds[i].getCommand( cmdName );
                    if ( cmd !== null ){
                        break;
                    }
                }
            }
            return cmd;
        },

       
        
        /**
         * Returns a string identifying this command as a composite.
         * @return {String} a string indicating this command is a composite.
         */
        getType : function(){
            return skel.Command.Command.TYPE_COMPOSITE;
        },
        
        isVisibleMenu : function(){
            var menuVisible = false;
            if ( this.m_cmds !== null ){
                for ( var i = 0; i < this.m_cmds.length; i++ ){
                    menuVisible = this.m_cmds[i].isVisibleMenu();
                    if ( menuVisible ){
                        break;
                    }
                }
            }
            return menuVisible;
        },
        
        isVisibleToolbar : function(){
            var toolVisible = false;
            if ( this.m_cmds !== null ){
                for ( var i = 0; i < this.m_cmds.length; i++ ){
                    toolVisible = this.m_cmds[i].isVisibleToolbar();
                    if ( toolVisible ){
                        
                        break;
                    }
                }
            }
            return toolVisible;
        },
        
        _resetEnabled : function( ){
            arguments.callee.base.apply(this, arguments);
            //The children should be enabled based on the state of the composite.
            var enabled = this.isEnabled();
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setEnabled( enabled );
            }
        },
       
        setVisibleMenu : function( visible){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setVisibleMenu( visible );
            }
        },
        
        setVisibleToolbar : function( visible){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setVisibleToolbar( visible );
            }
        },
        
        m_cmds : null
    }
});