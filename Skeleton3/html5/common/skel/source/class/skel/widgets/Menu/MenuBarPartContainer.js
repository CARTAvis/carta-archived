/**
 * Displays either a vertical container or horizontal container.
 *
 */

/**




 ************************************************************************ */

qx.Class.define("skel.widgets.Menu.MenuBarPartContainer", {
    extend : qx.ui.toolbar.PartContainer,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
    },

    members : {
        /**
         * Changes the orientation of the container from horizontal to vertical.
         * @param horizontal {boolean} true if the layout is horizontal; false for a vertical layout.
         */
        setLayoutHorizontal : function(horizontal) {
            if (horizontal) {
                this._setLayout(new qx.ui.layout.HBox());
            } else {
                this._setLayout(new qx.ui.layout.VBox());
            }
        }

    }

});
