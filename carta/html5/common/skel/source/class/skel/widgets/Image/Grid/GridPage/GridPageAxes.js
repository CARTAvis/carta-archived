/**
 * Displays controls for customizing the grid axes.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridPage.GridPageAxes", {
    extend : skel.widgets.Image.Grid.GridPage.GridPage,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, "Axes/Border");
    },

    members : {
        
        /**
         * Construct the page for managing axes color.
         */
        _makeColorPage : function(){
            this.m_colorPage = new skel.widgets.Image.Grid.Color.ColorPage("setAxesColor");
        },
        
        /**
         * Construct the page for axes settings.
         */
        _makeSettingsPage : function(){
            this.m_settingsPage = new skel.widgets.Image.Grid.Settings.SettingsAxesPage();
        },
        
        
        /**
         * Update the GUI from the server when the axes color has changed.
         */
        _setColorControls : function( ){
            if ( this.m_colorPage !== null ){
                this.m_colorPage.setControls( this.m_controls.grid.axes );
            }
        },
        
        /**
         * Update the settings from the server when the axes settings have changed.
         */
        _setSettingControls : function( ){
            if ( this.m_settingsPage !== null ){
                this.m_settingsPage.setControls( this.m_controls );
            }
        }

    }
});