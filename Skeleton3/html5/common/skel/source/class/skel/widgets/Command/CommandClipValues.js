/**
 * Container for commands that clip to a specific value.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandClipValues", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLIP_VALUE;
        this.base( arguments, "Clips", null );
        this.m_clips = [];
        
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
            var pathDict = skel.widgets.Path.getInstance();
            this.m_sharedVarClips = this.m_connector.getSharedVar(pathDict.CLIPS);
            this.m_sharedVarClips.addCB(this._clipPercentsChangedCB.bind(this));
            this._clipPercentsChangedCB();
        }
    },
    
    members : {
        
        /**
         * Add the server-side id of a window that has become active.
         * @param winId {String} the server-side id of an active window.
         */
        addActiveWindow : function( winId ){
            for ( var i = 0; i < this.m_clips.length; i++ ){
                this.m_clips[i].addActiveWindow( winId );
            }
        },
        
        _clipPercentsChangedCB : function() {
            var val = this.m_sharedVarClips.get();
            if ( val ){
                try {
                    this.m_clips = [];
                    var clips = JSON.parse( val );
                    var clipCount = clips.clipCount;
                    for ( var i = 0; i < clipCount; i++ ){
                        var clipAmount = clips.clipList[i];
                        clipAmount = clipAmount * 100;
                        var clipLabel = clipAmount.toString() + "%";
                        var cmd = new skel.widgets.Command.CommandClipValue( clipLabel );
                        this.m_clips[i] = cmd;
                    }
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        

        getCommand : function( cmdName ){
            var cmd = null;
            if ( this.isMatch( cmdName )){
                cmd = this;
            }
            else if ( this.m_clips !== null ){
                for ( var i = 0; i < this.m_clips.length; i++ ){
                    if ( this.m_clips[i].isMatch( cmdName )){
                        cmd = this.m_clips[i];
                        break;
                    }
                }
            }
            return cmd;
        },
        
        getValue : function(){
            var clipCmds = [];
            if ( this.m_clips !== null ){
                for ( var i = 0; i < this.m_clips.length; i++ ){
                    clipCmds[i] = this.m_clips[i];
                }
            }
          
            return clipCmds;
        },
        
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_GROUP;
        },
                
        isVisibleMenu : function(){
            var menuVisible = true;
            for ( var i = 0; i < this.m_clips.length; i++ ){
                menuVisible = this.m_clips[i].isVisibleMenu();
                if ( !menuVisible ){
                    break;
                }
            }
            return menuVisible;
        },
        
        isVisibleToolbar : function(){
            var toolVisible = true;
            for ( var i = 0; i < this.m_clips.length; i++ ){
                toolVisible = this.m_clips[i].isVisibleToolbar();
                if ( !toolVisible ){
                    break;
                }
            }
            return toolVisible;
        },
        
        /**
         * Remove the server-side id of a window that is no longer active.
         * @param winId {String} the server-side id of a window that is no longer active.
         */
        removePassiveWindow : function( winId ){
            for ( var i = 0; i < this.m_clips.length; i++ ){
                this.m_clips[i].removePassiveWindow( winId );
            }
        },
       
        setVisibleMenu : function( visible){
            for ( var i = 0; i < this.m_clips.length; i++ ){
                this.m_clips[i].setVisibleMenu( visible);
            }
        },
        
        setVisibleToolbar : function( visible){
            for ( var i = 0; i < this.m_clips.length; i++ ){
                this.m_clips[i].setVisibleToolbar( visible);
            }
        },
       
        m_sharedVarClips : null,
        m_clips : null
    }
});