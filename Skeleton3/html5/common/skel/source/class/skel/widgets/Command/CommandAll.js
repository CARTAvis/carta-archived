/**
 * Root command.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandAll", {
    extend : skel.widgets.Command.CommandComposite,
    type : "singleton",
    
    /**
     * Constructor
     */
    construct : function( ) {
        this.base( arguments, "All");
        this.m_clips = skel.widgets.Command.CommandClip.getInstance();
        this.m_layouts = skel.widgets.Command.CommandLayout.getInstance();
    },
    
    members : {
        
        isVisibleMenu : function(){
            var menuVisible = this.m_clips.isVisibleMenu();
            if ( menuVisible ){
                menuVisible = this.m_layouts.isVisibleMenu();
            }
            return menuVisible;
        },
        
        isVisibleToolbar : function(){
            var toolVisible = this.m_clips.isVisibleToolbar();
            if ( toolVisible ){
                toolVisible = this.m_clips.isVisibleToolbar();
            }
            return toolVisible;
        },
        
       
        setVisibleMenu : function( visible){
           this.m_clips.setVisibleMenu( visible );
           this.m_layouts.setVisibleMenu( visible );
        },
        
        setVisibleToolbar : function( visible){
            this.m_clips.setVisibleToolbar( visible );
            this.m_layouts.setVisibleToolbar( visible );
        },
        
       /**
        * Returns the command corresponding to the name if it matches the name of this
        * command or one of the children of this command; otherwise, returns null.
        * @param cmdName {String} the name of a command to look for.
        * @return {skel.widget.Command.Command} the corresponding command or null if no matching command exists in the
        *       children of this object.
        */
        getCommand : function( cmdName ){
            var cmd = arguments.callee.base.apply(this, arguments, cmdName );
            if ( cmd === null ){
                cmd = this.m_clips.getCommand( cmdName );
                if ( cmd === null ){
                    cmd = this.m_layouts.getCommand( cmdName );
                }
            }
            return cmd;
        },
        
        /**
         * Returns the child commands of this composite.
         * @return {Array} an array containing the immediate children of this command.
         */
        getValue : function(){
            var allCmds = [];
            allCmds[0] = this.m_clips;
            allCmds[1] = this.m_layouts;
           
            return allCmds;
        },
        
        m_clips : null,
        m_layouts : null
    }
});