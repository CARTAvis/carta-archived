/**
 * Displays statistics for a particular image.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 * @asset(skel/icons/minus.gif)
 * @asset(skel/icons/plus.gif)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.StatisticsImage", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.Statistics.StatisticsDisplayGenerator,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    
    events: {
        "imageChanged" : "qx.event.type.Data"
    },


    members : {
        
        /**
         * Clear the statistics display.
         */
        clear : function(){
            this.m_stats = [];
            this._updateStatsDisplay();
        },
        
        /**
         * Callback for a change in the collapsed status of the image statistics.
         */
        _collapseChanged : function(){
            var collapse = this._isCollapsed();
            this.setCollapse( !collapse );
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(2) );
            
            //Initialize the image name combo.
            var imageContainer = new qx.ui.container.Composite();
            this._add( imageContainer );
            var imageLabel = new qx.ui.basic.Label( "Image:");
            this.m_imageCombo = new skel.widgets.CustomUI.SelectBox( "", "");
            this.m_imageCombo.setToolTipText( "Select the image used to generate statistics.");
            skel.widgets.TestID.addTestId( this.m_imageCombo, "ImageStatsCombo");
            this.m_imageCombo.addListener( "selectChanged", function(){
                var selectIndex = this.m_imageCombo.getIndex();
                var data = {
                    index: selectIndex
                }
                this.fireDataEvent( "imageChanged", data );
            }, this );
            imageContainer.setLayout( new qx.ui.layout.HBox(2) );
            this.m_collapseToggle = new qx.ui.basic.Image( "skel/icons/plus.gif");
            this.m_collapseToggle.setToolTipText( "Expand/Collapse image statistics.");
            skel.widgets.TestID.addTestId( this.m_collapseToggle, "statsExpandCollapse" );  
            this.m_collapseToggle.addListener( "click", this._collapseChanged, this );
            
            imageContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            imageContainer.add( imageLabel );
            imageContainer.add( this.m_imageCombo, {flex:1} );
            imageContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            imageContainer.add( this.m_collapseToggle );
            
            //Initialize the container for image statistics.
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout( new qx.ui.layout.VBox(2) );
            this.setColumnCount( 4 );
        },
        
        /**
         * Return whether or not the image statistics are collapsed.
         * @return {boolean} - true if the image statistics are collapsed; false,
         *      otherwise.
         */
        _isCollapsed : function() {
            var collapse = false;
            var source = this.m_collapseToggle.getSource();
            if ( source == this.m_iconMinus){
                collapse = true;
            }
            return collapse;
        },
        
        /**
         * Send a command to the server to show/hide image statistics.
         * @param showStats {boolean} - true if image statistics should be shown;
         *      false otherwise.
         */
        _sendShowCmd : function( showStats){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND +  "setShowStatsImage";
                var params = "visible:"+showStats;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Set whether or not to collapse the image statistics.
         * @param collapse {boolean} - true if the image statistics should be collapsed;
         *      false otherwise.
         */
        setCollapse : function( collapse){
            var oldCollapse = this._isCollapsed();
            if ( collapse != oldCollapse ){
                if ( collapse ){
                    this.m_collapseToggle.setSource( this.m_iconMinus );
                }
                else {
                    this.m_collapseToggle.setSource( this.m_iconPlus );
                }
                this._updateStatsDisplay();
                this._sendShowCmd( !collapse );
            }
        },
        
        /**
         * Set the server-side id of the object managing statistics state.
         * @param id {String} - the server-side id of the object managing statistics state.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        
        /**
         * Store the list of keys that specifies the order for the statistics &
         * update the display accordingly.
         * @param keys {Array} - a list of ordered statistic labels.
         */
        setKeys : function( keys ){
            this.setKeyOrder( keys );
            this._updateStatsDisplay();
        },
        
        
        
        /**
         * Update the list of available images.
         * @param imageArray {Array} - a list of image names where statistics are
         *      available.
         */
        updateImages : function( imageArray ){
            var imageNames = [];
            //Cycle through each image
            for ( var i = 0; i < imageArray.length; i++ ){
                //The first element in each array is the image stats.
                var imageName = imageArray[i][0].Name;
                imageNames[i] = imageName;
            }
            this.m_imageCombo.setSelectItems( imageNames );
        },
        
        
        /**
         * Update the UI based on server image & region statistics.
         * @param stats {Object} - server-side object containing statistics for a
         *      particular region or image.
         */
        updateStats : function( stats ){
            this.m_stats = stats;
            this.m_imageCombo.setSelectValue( stats.Name );
            this._updateStatsDisplay();
        },
        
        /**
         * Update the UI with new statistics.
         */
        _updateStatsDisplay : function(){
            if ( this.m_stats !== null ){
                this.m_content.removeAll();
                var collapsed = this._isCollapsed();
                if ( this.m_stats !== null && !collapsed){
                    var content = this.generateStatsDisplay( this.m_stats );
                    this.m_content.add( content );
                }
            }
        },
        
        m_id : null,
        m_iconMinus : "skel/icons/minus.gif",
        m_iconPlus : "skel/icons/plus.gif",
        m_collapseToggle : null,
        m_content : null,
        m_imageCombo : null,
        m_connector : null,
        m_stats : null
    }
});