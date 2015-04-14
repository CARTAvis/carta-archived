/**
 * A command to add or remove a specific type of animation.
 */

qx.Class.define("skel.Command.Animate.CommandAnimate", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     */
    construct : function( label ) {
        this.base( arguments, label, null );
        this.m_toolBarVisible = false;
        this.setEnabled( false );
        this.setValue(false);
        this.setToolTipText( "Show/hide the " + this.getLabel() + " animator.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var emptyFunc = function(){};
                var animCmd = skel.Command.Animate.CommandAnimations.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( animCmd ) ){
                        var addRemVal = "addAnimator";
                        if ( !vals ){
                            addRemVal = "removeAnimator";
                        }
                        var path = skel.widgets.Path.getInstance();
                        var cmd = activeWins[i].getIdentifier() + path.SEP_COMMAND + addRemVal;
                        var params = "type:"+this.getLabel();
                        this.m_connector.sendCommand( cmd, params, emptyFunc);
                        break;
                    }
                }
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});