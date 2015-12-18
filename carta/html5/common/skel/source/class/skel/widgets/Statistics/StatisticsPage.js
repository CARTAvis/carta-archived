/**
 * Displays statistics for a particular image or region.
 */


qx.Class.define("skel.widgets.Statistics.StatisticsPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( title ) {
        this.base(arguments, title, "");
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            var grid = new qx.ui.layout.Grid(2, 2);
            for ( var i = 0; i < this.m_COL_COUNT; i=i+2 ){
                grid.setColumnAlign( i, "right", "middle");
            }
            this._setLayout( grid );
        },
        

        
        /**
         * Update the UI based on server image & region statistics.
         * @param stats {Object} - server-side object containing statistics for a
         *      particular region or image.
         */
        updateStats : function( stats ){
            var rowIndex = 0;
            var colIndex = 0;
            
            for ( var key in stats ){
                if ( stats.hasOwnProperty( key ) ){
                    if ( key !== "name"){
                        var label = new qx.ui.basic.Label( key +":");
                        var text = new qx.ui.form.TextField();
                        text.setValue( stats[key]);
                        text.setEnabled( false );
                        this._add( label, {row:rowIndex, column:colIndex} );
                        colIndex++;
                        this._add( text, {row:rowIndex, column:colIndex} );
                        colIndex++;
                        if ( colIndex == this.m_COL_COUNT ){
                            rowIndex++;
                            colIndex = 0;
                        }
                    }
                }
            }
        },
    
        m_COL_COUNT : 6
    }
});