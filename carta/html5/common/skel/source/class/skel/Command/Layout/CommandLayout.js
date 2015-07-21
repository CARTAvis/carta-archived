/**
 * Container for commands that change the layout.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Layout.CommandLayout", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Layout", null );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Layout.CommandLayoutImage.getInstance();
        this.m_cmds[1] = skel.Command.Layout.CommandLayoutAnalysis.getInstance();
        this.m_cmds[2] = skel.Command.Layout.CommandLayoutCustom.getInstance();
        this.setValue( this.m_cmds );
        
        //Listen to the layout so if a different one is selected on the server we can update the GUI.
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector.getSharedVar( pathDict.LAYOUT );
        this.m_sharedVar.addCB( this._layoutChangedCB.bind(this));
    },
    
    members : {
        
        _layoutChangedCB : function(){
            var layoutObjJSON = this.m_sharedVar.get();
            if ( layoutObjJSON ){
                try {
                    var layout = JSON.parse( layoutObjJSON );
                    var type = layout.layoutType;
                    if ( type === "Image"){
                        this.setValues( true, false, false );
                    }
                    else if ( type === "Analysis"){
                        this.setValues( false, true, false );
                    }
                    else if ( type === "Custom"){
                        this.setValues( false, false, true );
                    }
                    else {
                        console.log( "CommandLayout: unrecognized layout type: "+type);
                    }
                }
                catch( err ){
                    console.log( "CommandLayout, could not parse: "+layoutObjJSON );
                }
            }
        },
        
        /**
         * Set the children of this command active/inactive.
         * @param active {boolean} true if the command should be active; false otherwise.
         */
        //Written so that when a window is added or removed, the server-side update of
        //the value will not trigger a relayout.
        setActive : function( active ){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setActive( active );
            }
        },
        
        setValues : function( image, analysis, custom ){
            var imageCmd = skel.Command.Layout.CommandLayoutImage.getInstance();
            imageCmd.setValue( image );
            var analysisCmd = skel.Command.Layout.CommandLayoutAnalysis.getInstance();
            analysisCmd.setValue( analysis );
            var customCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
            customCmd.setValue( custom );
        },
        
        m_sharedVar : null
    }
});