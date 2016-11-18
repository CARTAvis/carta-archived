/**
 * Command to close a specific image that has been loaded.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandDataClose", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} name of the image to close.
     */
    construct : function( label, typeStr, closeWhat, id ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + closeWhat;
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = false;
        this.setEnabled( true );
        this.m_global = false;
        this.m_params = typeStr;
        this.m_paramValue = id;
      
        this.setToolTipText("Remove the "+typeStr+ ": " + this.getLabel() + ".");
    },
    
    members : {

        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var label = this.getLabel();
            if ( this.m_paramValue !== null && typeof this.m_paramValue !== "undefined"){
                label = this.m_paramValue;
            }
            var params = this.m_params + ":" + label;
            var errMan = skel.widgets.ErrorHandler.getInstance();
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var windowInfo = skel.Command.Command.m_activeWins[i];
                    var id = windowInfo.getIdentifier();
                    if ( this.m_params == "region"){
                    	id = windowInfo.getRegionIdentifier();
                    }
                    this.sendCommand( id, params, undoCB );
                }
                errMan.clearErrors();
            }
            else {
                errMan.updateErrors( "Error closing "+this.m_params +".");
            }
        },
        
        m_params : "image",
        m_paramValue: null
    }
});