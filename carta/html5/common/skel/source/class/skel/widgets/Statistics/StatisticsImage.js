/**
 * Displays statistics for a particular image.
 */


qx.Class.define("skel.widgets.Statistics.StatisticsImage", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.Statistics.StatisticsDisplayGenerator,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
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
            imageContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            imageContainer.add( imageLabel );
            imageContainer.add( this.m_imageCombo, {flex:1} );
            imageContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            
            //Initialize the container for image statistics.
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout( new qx.ui.layout.VBox(2) );
            this.setColumnCount( 4 );
            
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
                if ( this.m_stats !== null ){
                    var content = this.generateStatsDisplay( this.m_stats );
                    this.m_content.add( content );
                }
            }
        },
        
        
        
        m_content : null,
        m_imageCombo : null,
        m_stats : null
    }
});