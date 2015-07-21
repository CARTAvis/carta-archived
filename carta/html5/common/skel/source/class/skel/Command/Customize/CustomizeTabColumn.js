/**
 * The display of a particular command that allows the visibility to be changed.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 * @asset(skel/icons/user-desktop.png)
 ******************************************************************************/

qx.Class.define("skel.Command.Customize.CustomizeTabColumn", {
    extend : qx.ui.tree.TreeFolder,
    
    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments);
        this._init();
    },
    
    events : {
        cmdVisibilityChanged : 'qx.event.type.Data'
    },
    
    members : {

        /**
         * Initializes the UI.
         */
        _init : function() {
            
            //Indentation and tree-lines
            this.addSpacer();
            this.addOpenButton();
            
            //Tree icon
            this.addIcon();
            this.setIcon( "skel/icons/user-desktop.png");
            
            //Checkbox
            var checkbox = new qx.ui.form.CheckBox();
            checkbox.setFocusable( false );
            this.addWidget( checkbox );
            
            //Label
            this.addLabel( "");
            this.bind( "checked", checkbox, "value");
            checkbox.bind( "value", this, "checked");
            checkbox.addListener( skel.widgets.Path.CHANGE_VALUE, function(ev){
                var data = {
                    name : this.getLabel(),
                    checked : ev.getTarget().getValue()
                };
                this.fireDataEvent( "cmdVisibilityChanged", data);
                this._updateChildChecks(this.getChecked());
            }, this );
        },
        
        /**
         * Updates the checked status of child nodes based on this node.
         * @param checked {boolean} true if this node is checked; false otherwise.
         */
        _updateChildChecks : function(checked){
            var childArray = this.getChildren();
            for ( var i = 0; i < childArray.length; i++ ){
                childArray[i].setChecked( checked );
            }
        }
    },
    
    properties : {
        checked : {
            event: "changeChecked",
            nullable: true
        }
    }

});