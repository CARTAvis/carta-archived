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
         * Set the list of animators that are available for display based on the image.
         * @param animators {String} - a list of available animators.
         */
        setAnimations : function( animators ){
            this.m_cmds = [];
            //Make a command for each of the supported animations.
            for ( var j = 0; j < animators.length; j++ ){
                this.m_cmds[j] = new skel.Command.Animate.CommandAnimate(animators[j]);
            }
            this.setValue( this.m_cmds );
        }
    }
});