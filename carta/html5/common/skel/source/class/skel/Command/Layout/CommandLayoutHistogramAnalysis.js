/**
 * Command to change the layout to a preset analysis layout.
 */

qx.Class.define("skel.Command.Layout.CommandLayoutHistogramAnalysis", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setHistogramAnalysisLayout";
        this.base( arguments, "HistogramAnalysis Layout", cmd);
        this.m_toolBarVisible = false;
        this.setValue( false );
        this.setToolTipText("Set a predefined layout optimized for histogram analyzing an image.");
    },

    members : {

        doAction : function( vals, undoCB ){
            if ( this.m_active){
                var path = skel.widgets.Path.getInstance();
                this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, "", undoCB );
            }

        },

        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },

        /**
         * Sets whether or not this command's action will be performed.
         * @param active {boolean} true if the action should be performed; false otherwise.
         */
        setActive : function( active ){
            this.m_active = active;
        },

        m_active : true

    }
});
