/**
 * Container for commands that clip to a specific value.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Clip.CommandClipValues", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLIP_VALUE;
        this.base( arguments, "Clips", null );
        this.m_cmds = [];
        
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
         * Callback for a change in the available clip percentages.
         */
        _clipPercentsChangedCB : function() {
            var val = this.m_sharedVarClips.get();
            if ( val ){
                try {
                    this.m_cmds = [];
                    var clips = JSON.parse( val );
                    var clipCount = clips.clipCount;
                    for ( var i = 0; i < clipCount; i++ ){
                        var clipAmount = clips.clipList[i];
                        clipAmount = clipAmount * 100;
                        var clipLabel = clipAmount.toString() + "%";
                        var cmd = new skel.Command.Clip.CommandClipValue( clipLabel );
                        this.m_cmds[i] = cmd;
                    }
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        
       
        m_sharedVarClips : null
    }
});