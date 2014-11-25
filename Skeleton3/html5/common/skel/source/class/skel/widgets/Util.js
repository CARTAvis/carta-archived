/**
 * Utility class for determining the location of widgets.
 */

/* global qx */

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
            return widget.getBounds().left;
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
            return widgetBounds.left + widgetBounds.width;
        },

        /**
         * Returns the 'top' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getTop : function(widget) {
            return widget.getBounds().top;
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
            return widgetBounds.top + widgetBounds.height;
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
            var box = widget.getContentLocation("box");
            var left = 0;
            var top = 0;
            if ( box ){
                left = box.left;
                top = box.top;
            }
            var xCoord = left;
            var yCoord = top;
            if ( widgetBounds ){
                xCoord = left + Math.round(widgetBounds.width / 2);
                yCoord = top + Math.round(widgetBounds.height / 2);
            }
            return [ xCoord, yCoord ];
        },

        /**
         * Returns mouse event's local position (with respect to the event
         * target)
         *
         * Fixing getTarget() to getCurrentTarget(), but I am not sure how reliable this is.
         * I think the right way to do this is to get the "box" of the element on which you
         * received the event and subract it from the mouse x/y. Or you could pass the
         * box to this function.
         */
        localPos : function(event) {

            //var target = event.getTarget();
            var target = event.getCurrentTarget();
            var box = target.getContentLocation("box");
           
            var left = event.getDocumentLeft();
            var top = event.getDocumentTop();
            if ( box ){
                left = left - box.left;
                top = top - box.top;
            }
            
            return  {
                x : left,
                y : top
            };
        }

    }

});