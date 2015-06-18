/**
 * A custom color chooser.
 */

qx.Class.define( "skel.widgets.CustomUI.ColorSelector",
    {
        extend: qx.ui.control.ColorSelector,

        /**
         * Constructor.
         */
        construct: function ( ) {
            
            this.base( arguments);
            this.getChildControl( "hex-field-composite").exclude();
            this.getChildControl( "hsb-spinner-composite").exclude();
            this.getChildControl( "preview-field-set").exclude();
        },

        members: {
            
            _createChildControlImpl : function(id, hash) {
                var control;
                switch (id) {
                
                case "rgb-spinner-composite":
                    var layout = new qx.ui.layout.Grid();
                    control = new qx.ui.container.Composite(layout);
                  
                    var redLabel = new qx.ui.basic.Label( this.tr("Red:"));
                    control.add( redLabel, {row: 0, column:0});
                    control.add( this.getChildControl( "rgb-spinner-red"), {row:0, column:1});
                    
                    var greenLabel = new qx.ui.basic.Label( this.tr( "Green:"));
                    control.add( greenLabel, {row:1, column:0} );
                    control.add( this.getChildControl("rgb-spinner-green"), {row:1, column:1});
                    
                    var blueLabel = new qx.ui.basic.Label( this.tr( "Blue:"));
                    control.add( blueLabel, {row:2, column:0} );
                    control.add( this.getChildControl( "rgb-spinner-blue"), {row:2, column:1});
                    break;
                
                case "preset-grid":
                    var rowCount = 5;
                    var colCount = 5;
                    var controlLayout = new qx.ui.layout.Grid(rowCount, colCount);
                    control = new qx.ui.container.Composite(controlLayout);

                    var colorField;
                    var colorPos;

                    var rowIndex = 0;
                    var colIndex = 0;
                    for (var i=0; i<2; i++){
                      for (var j=0; j<10; j++){
                        colorPos = i * 10 + j;
                        rowIndex = Math.floor( colorPos / colCount );
                        colIndex = colorPos - rowIndex * rowCount;
                        colorField = this.getChildControl("colorbucket#" + colorPos);
                        colorField.setBackgroundColor(this.__presetTable[colorPos]);
                        control.add(colorField, {column: colIndex, row: rowIndex});
                      }
                    }
                    break;
                }
                return control || this.base(arguments, id, hash);
            }
        }

    } );

