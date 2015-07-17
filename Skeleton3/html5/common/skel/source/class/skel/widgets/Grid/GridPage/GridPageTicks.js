/**
 * Displays controls for customizing the grid ticks.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.GridPage.GridPageTicks", {
    extend : skel.widgets.Grid.GridPage.GridPage,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, "Ticks");
    },

    members : {
        
        /**
         * Construct the page for managing tick color.
         */
        _makeColorPage : function(){
            this.m_colorPage = new skel.widgets.Grid.Color.ColorPage("setTickColor");
        },
        
        /**
         * Construct the page for tick settings.
         */
        _makeSettingsPage : function(){
            this.m_settingsPage = new skel.widgets.Grid.Settings.SettingsTicksPage();
        },
        
        
        /**
         * Update the GUI from the server when the tick color has changed.
         */
        _setColorControls : function( ){
            if ( this.m_colorPage !== null ){
                this.m_colorPage.setControls( this.m_controls.grid.ticks );
            }
        },
        
        /**
         * Update the settings from the server when the tick settings have changed.
         */
        _setSettingControls : function( ){
            if ( this.m_settingsPage !== null ){
                this.m_settingsPage.setControls( this.m_controls );
            }
        }
    }
});