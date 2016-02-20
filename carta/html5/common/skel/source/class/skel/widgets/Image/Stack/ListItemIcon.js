/**
 * A list item with an in memory constructed icon consisting of a square with a
 * colored background.
 */

qx.Class.define("skel.widgets.Image.Stack.ListItemIcon", {
    extend : qx.ui.form.ListItem,

    /**
     * Constructor.
     */
    construct : function( label, color ) {
        this.base(arguments);
        this.setLabel( label );
        this.m_color = color;
        if ( this.m_color.length > 0 ){
            this.setIcon( this.m_color );
            this.setIconPosition( "right" );
        }
    },
    

    members : {
        
        /**
         * Constructs an in memory icon.
         * @param id {String} - an identifier for the widget to be constructed.
         */
        _createChildControlImpl : function( id ){
            var control;
            switch( id ){
                case "icon":
                    control = new qx.ui.core.Widget();
                    control.setBackgroundColor( this.m_color );
                    control.setDecorator( this.m_BORDER_LINE );
                    control.setWidth(10);
                    control.setHeight(10);
                    control.setMaxHeight(10);
                    this._add( control );
                    break;
            }
            return control || this.base( arguments, id );
        },
    
        m_color : null,
        m_BORDER_LINE : "line-border"

    }
});