/**
 * Lays out statistics for a particular image or region in an image.
 */

qx.Mixin.define("skel.widgets.Statistics.StatisticsDisplayGenerator", {

    
    members : {
        
        /**
         * Add a label to the content at the given row and column.
         * @param label {String} - the text of the label.
         * @param content {qx.ui.container.Composite} - the container should contain the label.
         * @param rowIndex {Number} - the row in the grid where the label should be added.
         * @param colIndex {Number} - the column in the grid where the label should be added.
         */
        _addLabel : function( label, content, rowIndex, colIndex ){
            var label = new qx.ui.basic.Label( label +":");
            label.setTextAlign( "right");
            content.add( label, {row:rowIndex, column:colIndex} );
        },
        
        /**
         * Add a text field to the content at the given row and column.
         * @param value {String} - the text to display in the field.
         * @param content {qx.ui.container.Composite} - the container should contain the label.
         * @param rowIndex {Number} - the row in the grid where the label should be added.
         * @param colIndex {Number} - the column in the grid where the label should be added.
         */
        _addText : function( key, value, content, rowIndex, colIndex ){
            var text = new qx.ui.form.TextField();
            skel.widgets.TestID.addTestId( text, key + "Stat");
            text.setValue( value );
            text.setReadOnly( true );
            content.add( text, {row:rowIndex, column:colIndex} );
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
            if ( this.m_keys !== null ){
                var rowIndex = 0;
                var colIndex = 0;
                //Loop through the keys with specified order.
                for ( var i = 0; i < this.m_keys.length; i++ ){
                    var key = this.m_keys[i];
                    var statsKey = this._getKey( stats, key.label );
                    if ( statsKey !== null ){
                        if ( statsKey !== "Name" && key.visible ){
                            this._addLabel( statsKey, content, rowIndex, colIndex );
                            colIndex++;
                            this._addText( statsKey, stats[statsKey], content, rowIndex, colIndex );
                            colIndex++;
                            if ( colIndex == this.m_colCount ){
                                rowIndex++;
                                colIndex = 0;
                            }
                        }
                    }
                }
            }
            
            return content;
        },
        
        /**
         * Looks for the key as a property of the passed in object.
         * @param stats {Object} - object containing statistics.
         * @param target {String} - the property to look for.
         */
        _getKey : function( stats, target ){
            var result = null;
            if ( typeof stats != 'undefined'){
                if ( stats.hasOwnProperty( target) ){
                    result = target;
                }
                else {
                    for ( var stat in stats ){
                        if ( stat.indexOf( target ) >= 0 ){
                            result = stat;
                            break;
                        }
                    }
                }
            }
            else {
                console.log( "stats was undefined");
            }
            return result;
        },
        
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
        
        m_keys : null,
        m_colCount : 6
    }
});