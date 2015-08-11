/**
 * Displays controls for customizing the grid labels.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridPage.GridPageLabel", {
    extend : skel.widgets.Image.Grid.GridPage.GridPage,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, "Labels");
    },

    members : {
        
        /**
         * Construct the page for managing grid label color.
         */
        _makeColorPage : function(){
            this.m_colorPage = new skel.widgets.Image.Grid.Color.ColorPage("setLabelColor");
        },
        
        /**
         * Construct the page for grid label settings.
         */
        _makeSettingsPage : function(){
            this.m_settingsPage = new skel.widgets.Image.Grid.Settings.SettingsLabelPage();
        },
        
        /**
         * Update the GUI from the server when the grid label color has changed.
         */
        _setColorControls : function(){
            if ( this.m_colorPage !== null ){
                this.m_colorPage.setControls( this.m_controls.grid.labels );
            }
        },
        
        /**
         * Update the settings from the server when the grid label settings have changed.
         */
        _setSettingControls : function( ){
            if ( this.m_settingsPage !== null ){
                this.m_settingsPage.setControls( this.m_controls );
            }
        }

    }
});