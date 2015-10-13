/**
 * Displays controls for customizing the grid canvas.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridPage.GridPageCanvas", {
    extend : skel.widgets.Image.Grid.GridPage.GridPage,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, "Canvas");
    },

    members : {
        
        /**
         * Construct the page for managing canvas color.
         */
        _makeColorPage : function(){
            this.m_colorPage = new skel.widgets.Image.Grid.Color.ColorThemePage();
        },
        
        /**
         * Construct the page for canvas settings.
         */
        _makeSettingsPage : function(){
            this.m_settingsPage = new skel.widgets.Image.Grid.Settings.SettingsCanvasPage();
        },
        
        /**
         * Update the GUI from the server when the canvas color has changed.
         */
        _setColorControls : function( ){
            if ( this.m_colorPage !== null ){
                this.m_colorPage.setControls( this.m_controls.grid.theme );
            }
        },
        
        /**
         * Update the settings from the server when the canvas settings have changed.
         */
        _setSettingControls : function( ){
            if ( this.m_settingsPage !== null ){
                this.m_settingsPage.setControls( this.m_controls );
            }
        }

    }
});