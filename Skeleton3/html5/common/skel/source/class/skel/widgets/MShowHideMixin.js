/**
 * Show/hide animation for classes implementing the IHidable interface.
 */

/* global qx */

qx.Mixin.define("skel.widgets.MShowHideMixin", {

    members : {

        /**
         * Toggle on and off whether the hidable should always be shown or whether
         * its visibility should be determined based on proximity.
         * @param hidable {skel.widgets.IHidable} the animation object.
         * @param alwaysVisible {boolean} true if the animation object should always
         * be shown; false otherwise.
         */
        setAlwaysVisible : function(hidable, alwaysVisible) {
            if (alwaysVisible != this.m_alwaysVisible) {
                //Trigger a show/hide so it appears to take affect immediately
                this.show(hidable, !this.m_alwaysVisible);
                this.m_alwaysVisible = alwaysVisible;
            }
        },

        isAlwaysVisible : function() {
            return this.m_alwaysVisible;
        },

        /**
         * Trigger an animation that shows/hides the animation object.
         * @param hidable {skel.widgets.IHidable} the animation object.
         * @param show {boolean} true if the animation object should become visible;
         * false otherwise.
         */
        show : function(hidable, show) {
            if (show != this.m_shown && !this.m_alwaysVisible) {
                this.m_shown = show;
                var duration = 2;
                var frame = new qx.bom.AnimationFrame();
                frame.on("frame", function(timePassed) {
                    var percent = timePassed / duration;
                    hidable.animateSize(percent, show);
                }, this);
                frame.on("end", function() {
                    if (show) {
                        hidable.addWidgets();
                    }
                    hidable.animateEnd(show);
                }, this);
                if (!show) {
                    hidable.removeWidgets();
                }
                frame.startSequence(duration);
            }
        },

        /*
         * Hides or shows the hidable based on the location of the mouse.
         * Mouse close to the hidable = show; Otherwise, hide.
         * @param hidable the animation object.
         * @param ev the mouse event.
         */
        showHide : function(hidable, mouseLoc, widgetLoc) {
            if (!this.m_alwaysVisible) {
                //Show if we are close to the hidable or if we are in the bounds of
                //the hidable.
                var makeVisible = false;
                if (Math.abs(widgetLoc - mouseLoc) < this.m_mouseMargin) {
                    makeVisible = true;
                }
                this.show(hidable, makeVisible);

            }
        },

        // setting default to true because it causes annoying flicker and very high CPU
        // on my computer (pavol)
        m_alwaysVisible : true,
        m_shown : true,
        m_animationSize : 30,
        m_mouseMargin : 30

    }

});
