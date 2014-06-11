/**
 * Utility class for determining the location of widgets.
 */

qx.Class.define("skel.widgets.Util",
 {
	type: "static",
   statics:
   {
	   /**
	    * Returns the 'left' position of the widget.
	    * @param widget the widget whose position is to be determined.
	    */
	  getLeft : function( widget ){
       	var widgetBounds = widget.getBounds();
       	var widgetLeft = widgetBounds["left"];
       	return widgetLeft;
       },
       
       /**
	    * Returns the 'right' position of the widget.
	    * @param widget the widget whose position is to be determined.
	    */
      getRight : function( widget ){
       	var widgetBounds = widget.getBounds();
       	var widgetRight = widgetBounds["left"] + widgetBounds["width"];
       	return widgetRight;
       },
       
       /**
	    * Returns the 'top' position of the widget.
	    * @param widget the widget whose position is to be determined.
	    */
       getTop : function( widget ){
       	var widgetBounds = widget.getBounds();
       	var widgetTop = widgetBounds["top"];
       	return widgetTop;
       },
       
       /**
	    * Returns the 'bottom' position of the widget.
	    * @param widget the widget whose position is to be determined.
	    */
       getBottom : function( widget ){
       	var widgetBounds = widget.getBounds();
       	var widgetBottom = widgetBounds["top"] + widgetBounds["height"];
       	return widgetBottom;
       }
    
   }
});