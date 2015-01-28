/**
 * Container for clip commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandClip", {
    extend : skel.widgets.Command.CommandComposite,
    type : "singleton",

    construct : function( ) {
        this.base( arguments, "Clipping" );
        this.m_clipAuto = skel.widgets.Command.CommandClipAuto.getInstance();
        this.m_clipValues = skel.widgets.Command.CommandClipValues.getInstance();
    },
    
    members : {
        
        isVisibleMenu : function(){
            var menuVisible = this.m_clipAuto.isVisibleMenu();
            if ( menuVisible ){
                menuVisible = this.m_clipValues.isVisibleMenu();
            }
            return menuVisible;
        },
        
        isVisibleToolbar : function(){
            var toolVisible = this.m_clipAuto.isVisibleToolbar();
            if ( toolVisible ){
                toolVisible = this.m_clipValues.isVisibleToolbar();
            }
            return toolVisible;
        },
        
       
        setVisibleMenu : function( visible){
           this.m_clipValues.setVisibleMenu( visible );
           this.m_clipAuto.setVisibleMenu( visible );
        },
        
        setVisibleToolbar : function( visible){
            this.m_clipValues.setVisibleToolbar( visible );
            this.m_clipAuto.setVisibleToolbar( visible );
        },
        
       
        
        getCommand : function( cmdName ){
            var cmd = arguments.callee.base.apply(this, arguments, cmdName );
            if ( cmd === null ){
                if ( this.m_clipValues.isMatch( cmdName )){
                    cmd = this.m_clipValues;
                }
                else if ( this.m_clipAuto.isMatch( cmdName) ){
                    cmd = this.m_clipAuto;
                }
            }
            return cmd;
        },
        
        getValue : function(){
            var clipCmds = [];
            clipCmds[0] = this.m_clipAuto;
            clipCmds[1] = this.m_clipValues;
           
            return clipCmds;
        },
        

        m_sharedVarClips : null,
        m_clipValues : null,
        m_clipAuto : null
    }
});