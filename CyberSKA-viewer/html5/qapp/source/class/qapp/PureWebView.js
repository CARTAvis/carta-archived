/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 09/04/13
 * Time: 3:09 PM
 * To change this template use File | Settings | File Templates.
 */

/**

 @ignore(fv.console.log)
  @ignore(fv.hub.*)
 @ignore(fv.GLOBAL_DEBUG)

 ************************************************************************ */

qx.Class.define("qapp.PureWebView",
    {
        extend: qx.ui.core.Widget,

        statics: {
            PNG: "PNG",
            JPEG: "JPEG"
        },

        /**
         * @param id {String} div id
         * @param viewName {String} the name of the pure web view.
         * @param format {String} Specifies compression for interactive/full quality
         *        eg: "JPEG 30/PNG" for jpeg-30 interactive, PNG full quality
         */
        construct: function (id, viewName, format) {

            this.base(arguments);
            this.m_id = id;
            this.m_viewName = viewName;
            this.m_format = format;
            if (this.m_format === undefined)
                this.m_format = qapp.PureWebView.PNG;

            this.getContentElement().setAttributes(
                {
                    id: this.m_id,
                    "class": "pureweb"
                });

            var appearListenerId = this.addListener("appear", function (e) {

                fv.hub.emit("ui.pureWebView.created", {
                    id: this.m_id,
                    viewName: this.m_viewName,
                    format: this.m_format
                });

                var div = this.getContentElement().getDomElement();
                var ww = this.getWidth();
                var hh = this.getHeight();
                fv.hub.emit("ui.pureWebView.resized",
                    { width: this.getWidth(),
                        height: this.getHeight(),
                        viewName: this.m_viewName
                    });
                this.removeListenerById(appearListenerId);

            }, this);

            this.addListener("resize", function (e) {
                // only fire resize event if the actual dom element has been created
                if (null == this.getContentElement().getDomElement()) return;
                var data = e.getData();
                data.viewName= this.m_viewName;
                fv.hub.emit("ui.pureWebView.resized", data);
            }, this);

/*
            this.addListener( "appear", function() {
                fv.console.log( "appear-" + this.m_id);
            }, this);

            this.addListener( "disappear", function() {
                fv.console.log( "disappear-" +this.m_id);
            }, this);
*/
        },

        events: {
        },

        properties: {
        },

        members: {

            // overridden
            _createContentElement: function () {
                return new qx.html.Element("div", {
                    overflowX: "hidden",
                    overflowY: "hidden"
                });
            },

            m_id: null,
            m_viewName: null,
            m_format: null

        },

        destruct: function () {
        }

    });