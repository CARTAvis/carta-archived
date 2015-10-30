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
                this.restoreWindow(data.id);
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
         * Return the marked up html to display an informational message.
         * @param msg {String} - the information to display.
         * @return {String} the marked up html that displays the message.
         */
        _getMessageHtml : function( msg ){
            return this.m_BOLD_INFO + msg + this.m_BOLD_END;
        },
        
        /**
         * Return the marked up html to display an error message.
         * @param msg {String} - the error to display.
         * @return {String} the marked up html that displays the error message.
         */
        _getErrorHtml : function(errorMsg){
            return this.m_BOLD_ERROR + errorMsg + this.m_BOLD_END;
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
         * Show a list of messages to the user.
         * @param msgs {String} a list of user messages, each one separated by a '#' symbol.
         */
        showMessages : function( msgs ){
            var msgList = msgs.split( "#");
            var htmlText = "";
            for ( var i = 0; i < msgList.length; i++ ){
                if ( msgList[i].indexOf( "Error") >= 0 || msgList[i].indexOf( "Warn") >= 0 ){
                    htmlText = htmlText + this._getErrorHtml( msgList[i] );
                }
                else {
                    var infoMsg = msgList[i];
                    //Remove the info header if it exists.
                    var header = "Info:";
                    var infoIndex = msgList[i].indexOf( header );
                    if ( infoIndex === 0 ){
                        infoMsg = infoMsg.substr( header.length, infoMsg.length - header.length );
                    }
                    htmlText = htmlText + this._getMessageHtml( infoMsg );
                }
            }
            this._showRichText( htmlText );
        },
        

        
        /**
         * Display error messages on the status bar.
         * @param errorMessages {String} error messages.
         */
        showErrors : function(errorMessages) {
            var richText = this._getErrorHtml( errorMessages );
            this._showRichText( richText );
        },
        
        /**
         * Display informational messages on the status bar..
         * @param info {String} information message.
         */
        showInformation : function ( info ){
            var richText = this._getMessageHtml( info );
            this._showRichText( richText );
        },
        
        /**
         * Display marked up html on the status bar.
         * @param richText {String} the marked up html to display.
         */
        _showRichText : function( richText ){
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
            } 
            else if (error) {
                this.showErrors(error);
            }
        },

        /**
         * Initiate or revoke a shared session.
         * @param sessionShared {Boolean} whether the session should be shared or unshared.
         */
        updateSessionSharing : function(msg) {
            var con = mImport("connector");
            var statusCopy = this;
            if (msg.getData().share) {
                con.shareSession(function(url, error) {
                    statusCopy._shareSessionCB(url, error);
                }, "a", null, 60*60*1000000000);
            } 
            else {
                con.unShareSession(function(error) {
                    statusCopy.showErrors(error);
                });
                this.setSharedUrl("");
            }
        },

        m_iconifiedWindows : null,
        m_statusMessage : null,
        m_sharedContainer : null,
        m_sharedUrl : null,
        
        m_BOLD_END : "</b>",
        m_BOLD_ERROR :"<b style='color:red'>",
        m_BOLD_INFO : "<b style='color:black'>"
    },

    properties : {
        appearance : {
            refine : true,
            init : "status-bar"
        }

    }

});
