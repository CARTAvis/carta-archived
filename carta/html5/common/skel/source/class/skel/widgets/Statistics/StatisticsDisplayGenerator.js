/**
 * Lays out statistics for a particular image or region in an image.
 */

qx.Mixin.define("skel.widgets.Statistics.StatisticsDisplayGenerator", {

    
    members : {
        
        /**
         * Set the number of columns (both labels and values) in the display.
         * @param count {Number} - the number of columns in the layout.
         */
        setColumnCount : function( count ){
            this.m_colCount = count;
        },
        
        /**
         * Set the order of the keys in the layout.
         * @param keys {Array} - an ordered list of statistics keys to be displayed.
         */
        setKeyOrder : function( keys ){
            this.m_keys = keys;
        },
        
        
        /**
         * Update the UI based on server image & region statistics.
         * @param stats {Object} - server-side object containing statistics for a
         *      particular region or image.
         */
        generateStatsDisplay : function( stats ){
            var content = new qx.ui.container.Composite();
            content.setPadding( 0, 0, 0, 0 );
            content.setMargin( 1, 1, 1, 1 );
            var grid = new qx.ui.layout.Grid(2, 2);
            for ( var i = 0; i < this.m_colCount; i=i+2 ){
                grid.setColumnAlign( i, "right", "middle");
                grid.setColumnFlex(i+1, 1 );
            }
            content.setLayout( grid );
            
            var rowIndex = 0;
            var colIndex = 0;
            
            for ( var key in stats ){
                if ( stats.hasOwnProperty( key ) ){
                    if ( key !== "name"){
                        var label = new qx.ui.basic.Label( key +":");
                        label.setTextAlign( "right");
                        var text = new qx.ui.form.TextField();
                        text.setValue( stats[key]);
                        text.setEnabled( false );
                        content.add( label, {row:rowIndex, column:colIndex} );
                        colIndex++;
                        content.add( text, {row:rowIndex, column:colIndex} );
                        colIndex++;
                        if ( colIndex == this.m_colCount ){
                            rowIndex++;
                            colIndex = 0;
                        }
                    }
                }
            }
            return content;
        },
        
        m_keys : null,
        m_colCount : 6
    }
});