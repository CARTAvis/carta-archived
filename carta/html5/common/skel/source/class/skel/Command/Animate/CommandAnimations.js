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
         * Returns whether there is an existing animation command with the given name.
         * @param animateName {String} - an identifier for a type of animation.
         * @return {boolean} - true if there is already an animation command with the given
         *      name; false otherwise.
         */
        isExisting : function( animateName ){
            var existing = false;
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                if ( this.m_cmds[i].getLabel() == animateName ){
                    existing = true;
                    break;
                }
            }
            return existing;
        },
        
        
        /**
         * Set the list of animators that are available for display based on the image.
         * @param animators {String} - a list of available animators.
         */
        setAnimations : function( animators ){
            
            //Make a command for each of the supported animations if we don't already have one.
            for ( var j = 0; j < animators.length; j++ ){
                if ( ! this.isExisting( animators[j] ) ){
                    this.m_cmds.push( new skel.Command.Animate.CommandAnimate(animators[j]) );
                }
            }
            
            //Remove any animators that are no longer supported.
            for ( var i = this.m_cmds.length-1; i>= 0; i-- ){
                var removal = true;
                var cmdLabel = this.m_cmds[i].getLabel();
                for ( var j = 0; j < animators.length; j++ ){
                    if ( cmdLabel == animators[j] ){
                        removal = false;
                        break;
                    }
                }
                if ( removal ){
                    this.m_cmds.splice( i, 1 );
                }
            }
            this.setValue( this.m_cmds );
        }
    }
});