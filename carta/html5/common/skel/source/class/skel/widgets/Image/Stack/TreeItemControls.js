/**
 * Displays controls for moving layers up and down in the stack and grouping/ungrouping layers.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.TreeItemControls", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Stack", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        
        /**
         * Add or remove the control for combining layers into a group or splitting
         * layers from a group.
         * @param add {boolean} - true if the control should be added; false otherwise.
         */
        _addGroupCheck : function( add ){
            var groupIndex = this._indexOf( this.m_groupCheck );
            if ( add ){
                if ( groupIndex < 0 ){
                    this._add( this.m_groupCheck );
                    this._add( this.m_lastSpacer, {flex:1});
                }
            }
            else {
                if ( groupIndex >= 0 ){
                    this._remove( this.m_groupCheck );
                    this._remove( this.m_lastSpacer );
                }
            }
        },
        

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout( new qx.ui.layout.HBox(1));
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            //Up button
            this.m_upButton = new qx.ui.form.Button( "Up");
            this.m_upButton.addListener( "execute", function(){
                this._sendMoveCmd( false );
            }, this );
            this.m_upButton.setToolTipText( "Move the selected layer(s) one position up in the stack.");
            this._add( this.m_upButton );
            
            //Down button
            this.m_downButton = new qx.ui.form.Button( "Down");
            this.m_downButton.addListener( "execute", function(){
                this._sendMoveCmd( true );
            }, this );
            this.m_downButton.setToolTipText( "Move the selected layer(s) one position down in the stack.");
            this._add( this.m_downButton );
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            //Group check
            this.m_groupCheck = new qx.ui.form.CheckBox( "Group"); 
            this.m_groupCheck.setToolTipText( "Group/ungroup the selected layer(s).");
            skel.widgets.TestID.addTestId( this.m_groupCheck, "stackGroupImages");
            this.m_groupListenId = this.m_groupCheck.addListener( "changeValue", this._sendGroupCmd, this );
            this._add( this.m_groupCheck );
            this.m_lastSpacer = new qx.ui.core.Spacer();
            this._add( this.m_lastSpacer, {flex:1});
        },
        

        
        /**
         * Send a command to the server to group/ungroup selected layers.
         */
        _sendGroupCmd : function(){
            if ( this.m_id !== null ){
                var groupSelections = this.m_groupCheck.getValue();
                var params = "group:"+groupSelections;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGroup";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        
        /**
         * Send a command to the server to reorder the images in the stack.
         * @param msg {Array} - a list specifying the new image order.
         */
        _sendMoveCmd : function( moveDown ){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "moveLayers";
                var params = "moveDown:"+moveDown;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        
        /**
         * Send a command to the server to get the stack control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            this.m_id = imageId;
           
        },
        
        /**
         * Update the group layers control and the layer settings based on
         * the tree item(s) selected.
         * @param groupSelected {boolean} - true if composite nodes are selected.
         * @param siblingsSelected {boolean} - true if sibling children of the root are selected.
         * @param stackNodeSelected {boolean} - whether or not the move up and down the stack
         *      buttons should be enabled.
         */
        _treeItemSelected : function( groupSelected, siblingsSelected, upDownEnabled ){
            this.m_upButton.setEnabled( upDownEnabled );
            this.m_downButton.setEnabled( upDownEnabled );
            if ( groupSelected ){
                //Enable the group check box and select it.
                this.m_groupCheck.removeListenerById( this.m_groupListenId );
                this._addGroupCheck( true );
                this.m_groupCheck.setValue( true );
                this.m_groupListenId = this.m_groupCheck.addListener( "changeValue", this._sendGroupCmd, this );
            }
            else if ( siblingsSelected ){
                //Enable the group check box and unselect it.
                this.m_groupCheck.removeListenerById( this.m_groupListenId );
                this._addGroupCheck( true );
                this.m_groupCheck.setValue( false );
                this.m_groupListenId = this.m_groupCheck.addListener( "changeValue", this._sendGroupCmd, this );
            }
            else {
                //Disable the group check box
                this._addGroupCheck( false );
            }
        },
        
        
        m_id : null,
        m_connector : null,
        m_upButton : null,
        m_downButton : null,
        m_groupCheck : null,
        m_lastSpacer : null
      
    }
});