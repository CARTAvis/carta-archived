/**
 * Displays application and window specific menu items as well as a customizable
 * tool bar.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.MenuBar", {
    extend : qx.ui.toolbar.ToolBar,
    implement : skel.widgets.IHidable,
    include : skel.widgets.MShowHideMixin,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);

        this.m_menuPart = new skel.widgets.MenuBarPart;
        this.add(this.m_menuPart);
        this._initMenu();
        this.addListener("appear", this._setAnimationSize, this);

        this.addSpacer();
        
        this.m_toolPart = new skel.widgets.MenuBarPart;
        this.add( this.m_toolPart );
        this._initPresetTools();

        // Initially show the menu bar.
        this.setAlwaysVisible(this, true);
        this.setZIndex(1000);
    },

    events : {

        "layoutImage" : "qx.event.type.Data",
        "layoutImageAnalysisAnimator" : "qx.event.type.Data",
        "layoutRowCount" : "qx.event.type.Data",
        "layoutColCount" : "qx.event.type.Data",
        "menuAlwaysVisible" : "qx.event.type.Data",
        "menuMoved" : "qx.event.type.Data",
        "newWindow" : "qx.event.type.Data",
        "shareSession" : "qx.event.type.Data",
        "statusAlwaysVisible" : "qx.event.type.Data",
        /// emitted when cursor window toggle is clicked
        "showCursorWindow" : "qx.event.type.Data"
    },

    members : {



        /**
         * Ends the menu bar animation.
         * 
         * @param show
         *                {Boolean} true if the menu bar should
         *                be shown; false otherwise.
         */
        animateEnd : function(show) {
            if (show) {
                if (this.isTop()) {
                    var heightPercent = this.m_animationSize + "%";
                    this.setLayoutProperties({
                        bottom : heightPercent
                    });
                } else {
                    this.setWidth(this.m_animationSize);
                    this.setMinWidth(this.m_animationSize);
                    this.setMaxWidth(this.m_animationSize);
                }
            } else {
                if (this.isTop()) {

                    this.setLayoutProperties({
                        bottom : "100%"
                    });
                } else {
                    this.setWidth(0);
                    this.setMinWidth(0);
                    this.setMaxWidth(0);
                }
            }
        },

        /**
         * Resets the size of the menu bar based on the
         * percentage of the animation that has been completed
         * and whether the animation is proceeding to show or
         * hide the menu bar.
         * 
         * @param percent
         *                {Number} the progress of the
         *                animation.
         * @param show
         *                {Boolean} whether or not the menu bar
         *                is in the process of being shown.
         */

        animateSize : function(percent, show) {
            if (show) {
                if (this.isTop()) {
                    var calc = Math.round((1 - percent)
                            * (100 - this.m_animationSize)
                            + this.m_animationSize);
                    var calcStr = calc + "%";
                    this.setLayoutProperties({
                        bottom : calcStr
                    });
                } else {
                    var newSize = Math.round(percent
                            * this.m_animationSize);
                    this.setWidth(newSize);
                    this.setMinWidth(newSize);
                    this.setMaxWidth(newSize);
                }
            } else {
                if (this.isTop()) {
                    var leftOverSize = 100 - this.m_animationSize;
                    var calcNum = percent * leftOverSize
                            + this.m_animationSize;
                    var calc = Math.round(calcNum);
                    var calcStr = calc + "%";
                    this.setLayoutProperties({
                        bottom : calcStr
                    });
                } else {
                    var newSize = Math.round((1 - percent)
                            * this.m_animationSize);
                    this.setWidth(newSize);
                    this.setMinWidth(newSize);
                    this.setMaxWidth(newSize);
                }
            }
        },
        
        /**
         * Initializes the main menu.
         */
        _initMenu : function() {

            // Create "File" menu
            this.m_fileButton = new qx.ui.toolbar.MenuButton("Session");
            this.m_menuPart.add(this.m_fileButton);
            var fileMenu = new qx.ui.menu.Menu();
            var sessionButton = new qx.ui.menu.Button("Snapshot");
            fileMenu.add(sessionButton);
            fileMenu.add(new qx.ui.menu.Separator());
            var shareButton = new qx.ui.menu.CheckBox("Shared");
            shareButton.setValue(false);
            shareButton.addListener("execute", function() {
                this.fireDataEvent("shareSession", shareButton.getValue());
            }, this);
            fileMenu.add(shareButton);
            this.m_fileButton.setMenu(fileMenu);
            var sessionMenu = new qx.ui.menu.Menu();
            var saveSessionButton = new qx.ui.menu.Button("Save...");
            saveSessionButton.addListener("execute",
                    function() {
                        var connector = mImport("connector");
                        connector.sendCommand("/saveState",this.m_SAVE_STATE, function(val) {});
                    });
            sessionMenu.add(saveSessionButton);
            var restoreSessionButton = new qx.ui.menu.Button("Restore...");
            restoreSessionButton.addListener("execute",
                    function() {
                        var connector = mImport("connector");
                        connector.sendCommand("/restoreState", this.m_SAVE_STATE, function(val) {});
                    });
            sessionMenu.add(restoreSessionButton);
            sessionButton.setMenu(sessionMenu);

            // Create the "Layout" menu
            this.m_layoutButton = new qx.ui.toolbar.MenuButton("Layout");
            this.m_menuPart.add(this.m_layoutButton);
            var layoutMenu = new qx.ui.menu.Menu();
            var layoutImageButton = new qx.ui.menu.Button("Image");
            layoutImageButton.addListener("execute",
                    function() {
                        this.fireDataEvent("layoutImage", "");
                    }, this);
            layoutMenu.add(layoutImageButton);
            var layoutImageAnalysisControlsButton = new qx.ui.menu.Button(
                    "Image + Analysis + Animator");
            layoutImageAnalysisControlsButton.addListener(
                    "execute", function() {
                        this.fireDataEvent("layoutImageAnalysisAnimator","");
                    }, this);
            layoutMenu.add(layoutImageAnalysisControlsButton);
            var layoutCustomButton = new qx.ui.menu.Button(
                    "Custom...");
            layoutCustomButton.addListener("execute",
                    function() {
                        this._showCustomLayoutPopup();
                    }, this);
            layoutMenu.add(layoutCustomButton);
            var newWindowButton = new qx.ui.menu.Button("New Window...");
            newWindowButton.addListener("execute", function() {
                this.fireDataEvent("newWindow", "");
            }, this);
            layoutMenu.add(newWindowButton);
            this.m_layoutButton.setMenu(layoutMenu);

            // Create the "Preferences" menu
            this.m_prefButton = new qx.ui.toolbar.MenuButton("Preferences");
            this.m_menuPart.add(this.m_prefButton);
            var prefMenu = new qx.ui.menu.Menu();
            var menuButton = new qx.ui.menu.Button("Menu Bar");
            prefMenu.add(menuButton);
            var menuBarMenu = new qx.ui.menu.Menu();
            menuButton.setMenu(menuBarMenu);
            var menuShownButton = new qx.ui.menu.CheckBox("Always Visible");
            menuShownButton.setValue(true);
            menuShownButton.addListener("execute", function() {
                this.setAlwaysVisible(this, menuShownButton
                        .getValue());
                this.fireDataEvent("menuAlwaysVisible",
                        menuShownButton.getValue());
            }, this);
            menuBarMenu.add(menuShownButton);
            this.m_menuPositionButton = new qx.ui.menu.Button("Side");
            this.m_menuPositionButton.addListener("execute",
                    function() {
                        this._reposition();
                    }, this);
            menuBarMenu.add(this.m_menuPositionButton);
            this.m_prefButton.setMenu(prefMenu);

            var statusShownButton = new qx.ui.menu.CheckBox("Status Bar Always Visible");
            statusShownButton.addListener("execute",
                    function() {
                        this.fireDataEvent(
                                "statusAlwaysVisible",
                                statusShownButton.getValue());
                    }, this);
            prefMenu.add(statusShownButton);

            // Create the "Help" menu
            this.m_helpButton = new qx.ui.toolbar.MenuButton("Help");
            this.m_menuPart.add(this.m_helpButton);
            var helpMenu = new qx.ui.menu.Menu();
            helpMenu.add(new qx.ui.menu.Button("About..."));
            this.m_helpButton.setMenu(helpMenu);

        },
        
        /**
         * Initialize a tool bar containing 'quick' access buttons.
         */
        _initPresetTools : function() {
            var pathDict = skel.widgets.Path.getInstance();
            this.m_toolPart.add(new skel.boundWidgets.Toggle( "Cursor", "/hacks/cursorVisible"));
            this.m_toolPart.add(new skel.boundWidgets.Toggle( "Colormap", "/hacks/cm-windowVisible"));
            this.m_toolPart.add(new skel.boundWidgets.Toggle( "Recompute clips on new frame", pathDict.AUTO_CLIP));
            
            // add preset buttons
            var connector = mImport("connector");
            this.histClipVar = connector.getSharedVar( pathDict.CLIP_VALUE);
            var presetsContainer = new qx.ui.container.Composite(
                    new qx.ui.layout.HBox(5).set({alignX: "center"})
                );
            var radioGroup = new qx.ui.form.RadioGroup();
            radioGroup.setAllowEmptySelection(true);
            var presets = [ "95%", "98%", "99%", "99.5%", "99.9%", "99.99%", "100%" ];
            this.m_presetButtons = [];
            presets.forEach(function (e, ind) {
                var button = new qx.ui.toolbar.RadioButton(e).set({
                        toolTipText: "Set histogram to show " + e + " of the data.<br>" +
                            "Right click also automatically zooms in to the data."
                });
                button.addListener("execute", function () {
                        this.histClipVar.set( e);
                        button.setValue(true);
                }, this);
                button.addListener("mouseup", function (event) {
                        this.histClipVar.set( e);
                        button.setValue(true);
                }, this);
                radioGroup.add(button);
                button.setFocusable(false);
                presetsContainer.add(button);
                this.m_presetButtons.push(button);
            }, this);
            this.m_toolPart.add(presetsContainer);
        },

        /**
         * Returns whether or not the menu bar is currently
         * positioned at the top of the display area.
         */
        isTop : function() {
            var topPosition = true;
            // User has the option of changing it to top, which
            // means it is not
            // currently top.
            if (this.m_menuPositionButton.getLabel() == "Top") {
                topPosition = false;
            }
            return topPosition;
        },

        /**
         * Adds menu buttons to the application menu based on
         * the window that is selected.
         */
        addWindowMenu : function(ev) {
            if (this.m_windowButtons != null) {
                for (var i = 0; i < this.m_windowButtons.length; i++) {
                    this.m_menuPart
                            .remove(this.m_windowButtons[i]);
                }
            }
            this.m_windowButtons = ev.getData();
            for (var i = 0; i < this.m_windowButtons.length; i++) {
                this.m_menuPart.add(this.m_windowButtons[i]);
            }
        },

        /**
         * Change from a top menu to a left menu or vice versa.
         */
        reposition : function() {
            if (this.isTop()) {
                this.setHeight(30);
                this.setWidth(null);
                this.setMinWidth(null);
                this.setMaxWidth(null);
                this.setLayoutProperties({
                    left : "0%",
                    right : "0%",
                    top : "0%",
                    bottom : "100%"
                });
            } else {
                var menuWidth = 120;
                this.setWidth(menuWidth);
                this.setMinWidth(menuWidth);
                this.setMaxWidth(menuWidth);
                this.setHeight(null);
                this.setLayoutProperties({
                    left : "0%",
                    bottom : "0%",
                    top : "0%",
                    right : "90%"
                });
            }
        },

        /**
         * Sets the initial size information of the menu bar.
         */
        _setAnimationSize : function() {
            this.removeListener("appear",
                    this._setAnimationSize, this);
            if (this.isTop()) {
                var bounds = this.getBounds();
                var statusHeight = bounds["height"];
                var appBounds = this.getApplicationRoot()
                        .getBounds();
                var appHeight = appBounds["height"];
                this.m_animationSize = 100 - Math
                        .round(statusHeight / appHeight * 100);
                this.m_mouseMargin = 40;
            } else {
                this.m_animationSize = this.getWidth();
                this.m_mouseMargin = 100;
            }
        },

        /**
         * Removes widgets from the menu bar in preparation for hiding.
         */
        removeWidgets : function() {
            if (this.indexOf(this.m_menuPart) >= 0) {
                this.remove(this.m_menuPart);
            }
        },

        /**
         * Adds widgets to the menu bar in preparation for showing.
         */
        addWidgets : function() {
            if (this.indexOf(this.m_menuPart) == -1) {
                this.add(this.m_menuPart);
            }
        },

        /*
         * Hides or shows the status bar based on the location of the mouse.
         * Mouse close to screen top = show; Otherwise, hide.
         */
        showHideMenu : function(ev) {
            var widgetLoc = skel.widgets.Util.getTop(this);
            var mouseLoc = ev.getDocumentTop();
            if (!this.isTop()) {
                widgetLoc = skel.widgets.Util.getLeft(this);
                mouseLoc = ev.getDocumentLeft();
            }
            this.showHide(this, mouseLoc, widgetLoc);
        },

        _reposition : function() {
            var sideMenu = false;
            if (this.m_menuPositionButton.getLabel() == "Side") {
                this.m_menuPositionButton.setLabel("Top");
                sideMenu = true;
            } else {
                this.m_menuPositionButton.setLabel("Side");
            }
            this.removeWidgets();
            if (sideMenu) {
                this._setLayout(new qx.ui.layout.VBox(5));
            } else {
                this._setLayout(new qx.ui.layout.HBox(5));
            }
            this.m_menuPart.setLayoutHorizontal(!sideMenu);
            this.addWidgets();
            this.reposition();
            this.fireDataEvent("menuMoved");
            this._setAnimationSize();
        },

        /**
         * Shows a dialog allowing the user to choose the number of rows and columns
         * in the screen grid.
         */
        _showCustomLayoutPopup : function() {
            if (this.m_layoutCustom == null) {
                this.m_layoutCustom = new skel.widgets.CustomLayoutPopup();
                this.m_layoutCustom
                        .addListener(
                                "rowCount",
                                function(ev) {
                                    var rowCount = ev.getData();
                                    this.fireDataEvent(
                                            "layoutRowCount",
                                            rowCount);
                                }, this);
                this.m_layoutCustom
                        .addListener(
                                "colCount",
                                function(ev) {
                                    var colCount = ev.getData();
                                    this.fireDataEvent(
                                            "layoutColCount",
                                            colCount);
                                }, this);
            }
            var rightButton = skel.widgets.Util
                    .getRight(this.m_layoutButton);
            var topButton = skel.widgets.Util
                    .getTop(this.m_layoutButton);
            this.m_layoutCustom.placeToPoint({
                left : rightButton,
                top : topButton
            }, false);
            this.m_layoutCustom.show();
        },

        m_SAVE_STATE: "statename:firstSave",
        m_menuPart : null,
        m_toolPart : null,
        m_layoutCustom : null,
        m_fileButton : null,
        m_layoutButton : null,
        m_prefButton : null,
        m_helpButton : null,
        m_menuPositionButton : true
    }

});
