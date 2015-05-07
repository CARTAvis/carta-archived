/**
 * Manages a text area for displaying status messages and a tool bar for iconifying
 * windows.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Menu.StatusBar", {
    extend : qx.ui.core.Widget,

    construct : function() {
        this.base(arguments);

        this._setLayout(new qx.ui.layout.HBox(5));

        // Status Message
        this.m_statusMessage = new qx.ui.basic.Label("");
        skel.widgets.TestID.addTestId( this.m_statusMessage, "statusLabel"); 
        this.m_statusMessage.setRich( true );
        this.m_statusMessage.setAllowGrowX(true);
        this._add(this.m_statusMessage, {
            flex : 1
        });

        this.m_sharedContainer = new qx.ui.container.Composite();
        this.m_sharedContainer.setLayout(new qx.ui.layout.HBox(2));
        var sharedLabel = new qx.ui.basic.Label("Shared Session Url:");
        this.m_sharedUrl = new qx.ui.form.TextField();
        this.m_sharedUrl.setNativeContextMenu(true);
        this.m_sharedUrl.setReadOnly(true);
        this.m_sharedContainer.add(sharedLabel);
        this.m_sharedContainer.add(this.m_sharedUrl, {
            flex : 1
        });

        // Tool bar for holding iconified windows.
        this.m_iconifiedWindows = new qx.ui.toolbar.ToolBar();
        this.m_iconifiedWindows.setAllowGrowX(true);
        this._add(this.m_iconifiedWindows);
    },

    members : {
        
        /**
         * Adds a button to the status bar for restoring a minimized window.
         * @param ev {qx.event.type.Data} the data identifying the window
         *                being minimized.
         * @param restoreListener {skel.Application} the listener to be notified when
         *                the minimized window needs to be restored to its
         *                original location.
         */
        addIconifiedWindow : function(ev, restoreListener) {
            var data = ev.getData();
            var restoreTitle = data.title;
            if ( data.title === null ){
                restoreTitle = "Empty Window";
            }
            var menuButton = new qx.ui.toolbar.MenuButton("Restore: " + restoreTitle);
            menuButton.setAlignX("right");
            menuButton.setShowArrow(false);
            this.m_iconifiedWindows.add(menuButton);
            menuButton.addListener("execute", function() {
                this.restoreWindow(data.row, data.col);
            }, restoreListener);
            menuButton.addListener("execute", function() {
                this._removeIconifiedWindow(menuButton);
            }, this);
        },
        
        /**
         * Add status bar widgets.
         */
        addWidgets : function() {
            if (!this.hasLayoutChildren()) {
                this._add(this.m_statusMessage, {
                    flex : 1
                });
                this._add(this.m_iconifiedWindows);
                var url = this.m_sharedUrl.getValue();
                if (url && url.length > 0) {
                    this._add(this.m_sharedContainer, {
                        flex : 1
                    });
                }
            }
        },
        
        /**
         * Clear any messages posted on the status bar.
         */
        clearMessages : function(){
            this.m_statusMessage.setValue( "");
        },
        


        
        /**
         * Removes the button representing an iconfied window from the status
         * bar.
         * 
         * @param removeButton {qx.ui.toolbar.MenuButton} the button to remove.
         */
        _removeIconifiedWindow : function(removeButton) {
            if (removeButton !== null) {
                this.m_iconifiedWindows.remove(removeButton);
            }
        },
        
        /**
         * Remove all widgets from the status bar.
         */
        removeWidgets : function() {
            if (this.hasLayoutChildren()) {
                this._remove(this.m_statusMessage);
                this._remove(this.m_iconifiedWindows);
                if (this._indexOf(this.m_sharedContainer) >= 0) {
                    this._remove(this.m_sharedContainer);
                }
            }
        },
        
        /**
         * Display error messages on the status bar.
         * @param errorMessages {String} error messages.
         */
        showErrors : function(errorMessages) {
            var richText = "<b style='color:red'>"+errorMessages+"</b>";
            this.m_statusMessage.setValue(richText);
            if (!this.isVisible()) {
                this.show(this, true);
            }
        },
        
        /**
         * Display an informational message.
         * @param info {String} information message.
         */
        showInformation : function ( info ){
            var richText = "<b style='color:black'>"+info+"</b>";
            this.m_statusMessage.setValue(richText);
            if (!this.isVisible()) {
                this.show(this, true);
            }
        },
        
        /**
         * Add information to the status bar indicating the url of the shared
         * session.
         */
        setSharedUrl : function(value) {
            this.m_sharedUrl.setValue(value);
            if (this.isVisible()) {
                if (value.length > 0) {
                    this.removeWidgets();
                    this.addWidgets();
                } else if (this._indexOf(this.m_sharedContainer) >= 0) {
                    this._remove(this.m_sharedContainer);
                }
            }
        },

        
        /**
         * Set whether or not the status bar should always be visible or whether
         * it should be shown/hidden based on mouse location and/or status
         * change events.
         * 
         * @param alwaysVisible
         *                {boolean} true if the status window should always be
         *                visible; false otherwise.
         */
        setStatusAlwaysVisible : function(alwaysVisible) {
            //this.setAlwaysVisible(this, alwaysVisible);
        },

        /*
         * Hides or shows the status bar based on the location of the mouse.
         * Mouse close to screen bottom = show; Otherwise, hide. @param ev the
         * mouse event.
         */
        showHideStatus : function(ev) {
            /*var widgetLoc = skel.widgets.Util.getTop(this);
            var mouseLoc = ev.getDocumentTop();
            this.showHide(this, mouseLoc, widgetLoc);
*/
        },

        _shareSessionCB : function(url, error) {
            if (url !== null) {
                this.setSharedUrl(url);
            } else if (error) {
                this.showErrors(error);
            }
        },

        /**
         * Initiate or revoke a shared session.
         * @param sessionShared {Boolean} whether the session should be shared or unshared.
         */
        updateSessionSharing : function(sessionShared) {
            var con = mImport("connector");
            var statusCopy = this;
            if (sessionShared) {
                con.shareSession(function(url, error) {
                    statusCopy._shareSessionCB(url, error);
                }, "a", null, 60*60*1000000000);
            } else {
                con.unShareSession(function(error) {
                    statusCopy.showErrors(error);
                });
                this.setSharedUrl("");
            }
        },

        m_iconifiedWindows : null,
        m_statusMessage : null,
        m_sharedContainer : null,
        m_sharedUrl : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "status-bar"
        }

    }

});
