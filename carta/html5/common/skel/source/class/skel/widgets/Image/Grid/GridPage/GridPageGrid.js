/**
 * Displays controls for customizing the grid lines.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridPage.GridPageGrid", {
    extend : skel.widgets.Image.Grid.GridPage.GridPage,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, "Grid");
    },

    members : {
        
        /**
         * Construct the page for managing grid line color.
         */
        _makeColorPage : function(){
            this.m_colorPage = new skel.widgets.Image.Grid.Color.ColorPage("setGridColor");
        },
        
        /**
         * Construct the page for grid line settings.
         */
        _makeSettingsPage : function(){
            this.m_settingsPage = new skel.widgets.Image.Grid.Settings.SettingsGridPage();
        },
        
        /**
         * Update the GUI from the server when the grid line color has changed.
         */
        _setColorControls : function( ){
            if ( this.m_colorPage !== null ){
                this.m_colorPage.setControls( this.m_controls.grid.grid );
            }
        },
        
        /**
         * Update the settings from the server when the grid line settings have changed.
         */
        _setSettingControls : function( ){
            if ( this.m_settingsPage !== null ){
                this.m_settingsPage.setControls( this.m_controls );
            }
        }
    }
});