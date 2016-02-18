/**
 * Handles updating the tab state.
 */

/* global qx */

qx.Mixin.define("skel.widgets.MTabMixin", {

    members : {
        
        /**
         * Notify the server that a display tab has been selected.
         */
        _sendTabIndex : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var selectedPages = this.m_tabView.getSelection();
                if ( selectedPages.length > 0 ){
                    var selectedIndex = this.m_tabView.indexOf( selectedPages[0] );
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+"setTabIndex";
                    var params = "tabIndex:"+selectedIndex;
                    this.m_connector.sendCommand( cmd, params, null);
                }
            }
        },

        
        /**
         * Set the designated settings tab based on a server-side value.
         * @param index {Number} - the zero-based index of a settings tab.
         */
        _selectTab : function( index ){
            var oldSelection = this.m_tabView.getSelection();
            var oldIndex = -1;
            if ( oldSelection.length > 0 ){
                oldIndex = this.m_tabView.indexOf( oldSelection[0] );
            }
            if ( index != oldIndex ){
                this.m_tabView.removeListenerById ( this.m_tabListenId );
                var pages = [];
                pages[0] = this.m_pages[index];
                if ( pages.length == 1 ){
                    this.m_tabView.setSelection( pages );
                }
                this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            }
        },
        
        m_connector : null,
        m_id : null,
        m_pages : null,
        m_tabView : null,
        m_tabListenId : null
    }

});
