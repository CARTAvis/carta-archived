/**
 * Utility class for determining the location of widgets.
 */

qx.Class.define("skel.widgets.Util", {
    type : "static",
    statics : {
        /**
         * Returns the 'left' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getLeft : function(widget) {
            var widgetBounds = widget.getBounds();
            var widgetLeft = widgetBounds["left"];
            return widgetLeft;
        },

        /**
         * Returns the 'right' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getRight : function(widget) {
            var widgetBounds = widget.getBounds();
            var widgetRight = widgetBounds["left"] + widgetBounds["width"];
            return widgetRight;
        },

        /**
         * Returns the 'top' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getTop : function(widget) {
            var widgetBounds = widget.getBounds();
            var widgetTop = widgetBounds["top"];
            return widgetTop;
        },

        /**
         * Returns the 'bottom' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getBottom : function(widget) {
            var widgetBounds = widget.getBounds();
            var widgetBottom = widgetBounds["top"] + widgetBounds["height"];
            return widgetBottom;
        },

        /**
         * Returns an array of size 2 representing the point that is located at
         * that center of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose center is to be
         *                determined.
         */
        getCenter : function(widget) {
            var widgetBounds = widget.getBounds();
            var box = widget.getContentLocation();
            var xCoord = box.left + Math.round(widgetBounds["width"] / 2);
            var yCoord = box.top + Math.round(widgetBounds["height"] / 2);
            return [ xCoord, yCoord ];
        },

        /**
         * Returns mouse event's local position (with respect to the event
         * target)
         */
        localPos : function(event) {
            var target = event.getTarget();
            var box = target.getContentLocation("box");
            return {
                x : event.getDocumentLeft() - box.left,
                y : event.getDocumentTop() - box.top
            };
        }

    }

});