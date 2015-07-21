/**
 * Container for commands that determine what types of animation are available.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Animate.CommandAnimations", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Animate" );
        this.m_cmds = [];
        this.setValue( this.m_cmds );
        this.m_global = false;
        this.setEnabled( false );
        this.m_toolBarVisible = false;
        this._initAnimations();
        this.setToolTipText("Show/hide animators.");
    },
    
    
    members : {
        
        /**
         * Return the command with the corresponding label.
         * @param label {String} an identifier for an animator.
         * @return {skel.Command.Command} the command associated with the label.
         */
        getCmd : function( label ){
            var cmd = null;
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                if ( this.m_cmds[i].getLabel() === label ){
                    cmd = this.m_cmds[i];
                    break;
                }
            }
            if ( cmd === null ){
                console.log( "Unrecognized animation cmd for "+label );
            }
            return cmd;
        },
        
        /**
         * Set-up the shared variable that contains information about what animations
         * are available.
         */
        _initAnimations : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( path.ANIMATOR_TYPES );
            this.m_sharedVar.addCB( this._sharedVarCB.bind( this ));
            this._sharedVarCB();
        },
        
        /**
         * Callback for a state change; update the animators that are displayed.
         */
        _sharedVarCB : function( ){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    this.m_cmds = [];
                    try {
                        var animObj = JSON.parse( val );
                        //Make a command for each of the supported animations.
                        for ( var j = 0; j < animObj.animators.length; j++ ){
                            this.m_cmds[j] = new skel.Command.Animate.CommandAnimate(animObj.animators[j]);
                        }
                        this.setValue( this.m_cmds );
                    }
                    catch( err ){
                        console.log( "Could not parse: "+val );
                    }
                }
            }
        },
        
        m_sharedVar : null
    }
});