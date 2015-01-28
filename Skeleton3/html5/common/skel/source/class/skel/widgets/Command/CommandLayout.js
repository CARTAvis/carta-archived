/**
 * Container for commands that change the layout.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandLayout", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        this.base( arguments, "Layout", null );
        this.m_layoutAnalysis = skel.widgets.Command.CommandLayoutImage.getInstance();
        this.m_layoutImage = skel.widgets.Command.CommandLayoutAnalysis.getInstance();
    },
    
    members : {
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_GROUP;
        },
        
        isVisibleMenu : function(){
            var menuVisible = this.m_layoutImage.isVisibleMenu();
            if ( menuVisible ){
                menuVisible = this.m_layoutImage.isVisibleMenu();
            }
            return menuVisible;
        },
        
        isVisibleToolbar : function(){
            var toolVisible = this.m_layoutImage.isVisibleToolbar();
            if (toolVisible){
                toolVisible = this.m_layoutAnalysis.isVisibleToolbar();
            }
            
            return toolVisible;
        },
        
       
        setVisibleMenu : function( visible){
            this.m_layoutImage.setVisibleToolbar( visible );
            this.m_layoutAnalysis.setVisibleToolbar( visible );
        },
        
        setVisibleToolbar : function( visible){
            this.m_layoutImage.setVisibleToolbar( visible );
            this.m_layoutAnalysis.setVisibleToolbar( visible );
        },
        
        getCommand : function( cmdName ){
            var cmd = null;
            if ( this.isMatch(cmdName) ){
                cmd = this;
            }
            else {
                if ( this.m_layoutImage.isMatch( cmdName )){
                    cmd = this.m_layoutImage;
                }
                if ( cmd === null ){
                    if ( this.m_layoutAnalysis.isMatch( cmdName )){
                        cmd = this.m_layoutAnalysis;
                    }
                }
            }
            return cmd;
        },
        
        getValue : function(){
            var layoutCmds = [];
            layoutCmds[0] = this.m_layoutImage;
            layoutCmds[1] = this.m_layoutAnalysis;
            return layoutCmds;
        },
        

        m_layoutAnalysis : null,
        m_layoutImage : null
    }
});