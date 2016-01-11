/**
 * Displays statistics for the regions in an image
 */


qx.Class.define("skel.widgets.Statistics.StatisticsRegion", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.Statistics.StatisticsDisplayGenerator,


    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( label ) {
            this._setLayout(new qx.ui.layout.VBox(2));
            
            //Regions label
            var regionsLabel = new qx.ui.basic.Label( "Region:");
            this.m_regionsCombo = new skel.widgets.CustomUI.SelectBox( "", "");
            this.m_regionsCombo.addListener( "selectChanged", function(){
                this.m_selectIndex = this.m_regionsCombo.getIndex();
                this._statisticsChanged();
            }, this );
            var regionsContainer = new qx.ui.container.Composite();
            regionsContainer.setLayout( new qx.ui.layout.HBox(2) );
            regionsContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            regionsContainer.add( regionsLabel );
            regionsContainer.add( this.m_regionsCombo, {flex:1} );
            regionsContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            this._add( regionsContainer );
            
            //Region Statistics
            this.m_content = new qx.ui.container.Composite();
            this.m_content.setLayout( new qx.ui.layout.VBox(2) );
            this._add( this.m_content );
        },
        
        
        /**
         * Returns true if there are region statistics available for display;
         * false otherwise.
         * @return {boolean} - true if region statistics are available for display;
         *      false otherwise.
         */
        isStats : function(){
            var regionStats = false;
            if ( this.m_regionStats !== null && this.m_regionStats.length > 0 ){
                regionStats = true;
            }
            return regionStats;
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
         * Update the UI based on stored statistics information.
         */
        _statisticsChanged : function(){
            var statCount = this.m_regionStats.length;
            var regionNames = [];
            for ( var i = 0; i < statCount; i++ ){
                regionNames[i] = this.m_regionStats[i].Name;
            }
            this.m_regionsCombo.setSelectItems( regionNames );
            if ( this.m_selectIndex >= 0 && this.m_selectIndex < regionNames.length ){
                this.m_regionsCombo.setSelectValue( regionNames[this.m_selectIndex] );
            }
            this._updateStatsDisplay();
        },
        
        /**
         * Update the UI with new statistics.
         */
        _updateStatsDisplay : function(){
            if ( this.m_regionStats !== null ){
                var content = this.generateStatsDisplay( this.m_regionStats[this.m_selectIndex] );
                this.m_content.removeAll();
                this.m_content.add( content );
            }
        },
        
        /**
         * Store server statistics.
         * @param stats {Array} - statistics information from the server.
         */
        updateStats : function( stats ){
            this.m_regionStats = stats;
            this._statisticsChanged();
        },

        m_content : null,
        m_regionsCombo : null,
        m_selectIndex : 0,
        m_regionStats : null

    }
});