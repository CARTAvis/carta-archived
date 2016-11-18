/**
 * Container for region drawing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Region.CommandRegions", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Draw" );
        this.m_global = false;
        this.setEnabled(false);
        this.m_cmds = [];
        this.setValue( this.m_cmds );
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
            var pathDict = skel.widgets.Path.getInstance();
            this.m_sharedVarTypes = this.m_connector.getSharedVar(pathDict.REGION_TYPES);
            this.m_sharedVarTypes.addCB(this._regionTypesChangedCB.bind(this));
            this._regionTypesChangedCB();
        }
    },
    
    members : {
    	
    	/**
    	 * Update the region drawing commands based on the region types that are available on the
    	 * server.
    	 */
    	_regionTypesChangedCB : function(){
    		 var val = this.m_sharedVarTypes.get();
             if ( val ){
                 try {
                     this.m_cmds = [];
                     var regionTypes = JSON.parse( val );
                     var typeCount = regionTypes.types.length;
                     for ( var i = 0; i < typeCount; i++ ){
                         var regionType = regionTypes.types[i];
                         var cmd = new skel.Command.Region.CommandRegion( regionType );
                         this.m_cmds[i] = cmd;
                     }
                     this.setValue( this.m_cmds );
                     qx.event.message.Bus.dispatch(new qx.event.message.Message(
                         "commandsChanged", null));
                 }
                 catch( err ){
                     console.log( "Could not parse region types: "+val );
                     console.log( err );
                 }
             }
    	},
    	
    	 /**
         * Set the shape to draw.
         * @param val {String} - the shape to draw.
         */
        setDrawType : function ( val ){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                var drawType = this.m_cmds[i].getLabel();
                this.m_cmds[i].setServerSend( false );
                if ( drawType == val ){
                    this.m_cmds[i].setValue( true );
                }
                else {
                	this.m_cmds[i].setValue( false );
                }
                this.m_cmds[i].setServerSend( true );
            }
        },
        
    	
    	m_sharedVarTypes : null
    }
});
