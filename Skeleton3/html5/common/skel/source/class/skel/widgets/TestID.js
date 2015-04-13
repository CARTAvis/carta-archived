/**
 * Utility for enumerating unique html identifiers and adding them to the
 * page.
 */

qx.Class.define("skel.widgets.TestID", {
    type : "static",
    statics : {
        
        /**
         * Adds an 'id' attribute to the widget's html div.
         * @param widget {qx.ui.basic.Widget}.
         * @param testId {String} a unique id for locating the html element.
         */
        addTestId : function( widget, testId ){
            //Ids must be unique in the app so only use the id if we
            //have not already done so.
            var actualId = testId;
            if ( skel.widgets.TestID.widgetIds[testId] ){
                skel.widgets.TestID.widgetIds[testId] = skel.widgets.TestID.widgetIds[testId] + 1;
                actualId = actualId + skel.widgets.TestID.widgetIds[testId];
            }
            else {
                skel.widgets.TestID.widgetIds[testId] = 1;
            }

            //Testing Id
            widget.addListener("appear", function() {
                var container = this.getContentElement().getDomElement();
                container.id = actualId;
            }, widget );
        },
        
        widgetIds : {}
    }

});