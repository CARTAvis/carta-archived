/**
 * Displays either a left side menu bar or a top menu bar.
 */

/**
 ************************************************************************ */

qx.Class.define("skel.widgets.Menu.MenuBarPart", {
    extend : qx.ui.toolbar.Part,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
    },

    members : {
        /**
         * Change the orientation from horizontal to vertical.
         * @param horizontal {boolean} true for a top menu; false for a side menu.
         */
        setLayoutHorizontal : function(horizontal) {
            this.m_horizontal = horizontal;
            if (horizontal) {
                this._setLayout(new qx.ui.layout.HBox());
            } else {
                this._setLayout(new qx.ui.layout.VBox());
            }
            if (this.m_control !== null) {
                this.m_control.setLayoutHorizontal(this.m_horizontal);
            }
        },

        // overridden
        _createChildControlImpl : function(id, hash) {
            var control;

            switch (id) {
            case "handle":
                control = new qx.ui.basic.Image();
                control.setAlignY("middle");
                this._add(control);
                break;

            case "container":
                this.m_control = new skel.widgets.Menu.MenuBarPartContainer();
                this.m_control.setLayoutHorizontal(this.m_horizontal);
                this.m_control.addListener("syncAppearance",
                        this.__onSyncAppearance, this);
                this._add(this.m_control);
                this.m_control.addListener("changeChildren", function() {
                    this.__onSyncAppearance();
                }, this);
                break;
            }

            return this.m_control || this.base(arguments, id);
        },

        m_horizontal : true,
        m_control : null

    }

});
