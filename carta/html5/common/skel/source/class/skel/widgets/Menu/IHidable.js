/**
 * Implemented by classes that can show/hide themselves depending on the location
 * of the cursor (menubar, statusbar).
 */

/**





 ************************************************************************ */

qx.Interface.define("skel.widgets.Menu.IHidable", {

    members : {

        /**
         * Remove display widgets.
         */
        removeWidgets : function() {
        },

        /**
         * Add display widgets.
         */
        addWidgets : function() {
        },

        /**
         * Resize the animation object.
         * @param percent {Number} how close the animation is to completion.
         * @param show {boolean} whether the animation object is being shown or hidden.
         */
        animateSize : function(percent, show) {
        },

        /**
         * Notification that the animation has completed.
         * @param show {boolean} whether the animation object is now shown or hidden.
         */
        animateEnd : function(show) {
        }
    }

});
