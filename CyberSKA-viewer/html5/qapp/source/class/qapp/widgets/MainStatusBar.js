/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 12/11/13
 * Time: 10:49 AM
 *
 * The MainStatusBar is the widget displayed at the bottom of the main window. It shows
 * information related to the cursor (position, frozen status, etc)
 */

/* global qx, qapp, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)
 */


qx.Class.define("qapp.widgets.MainStatusBar",
    {
        extend: qx.ui.container.Composite,

        /**
         * Constructor
         * @param labelPrefix {String}
         * @param labelPostfix {String|Function}
         * @param varPath {String} Path to the bound shared variable
         */
        construct: function () {

            this.base(arguments, new qx.ui.layout.HBox(5));
            this.set( {
//                allowGrowX: true,
//                allowGrowY: true,
                textColor: "rgb(255,255,255)",
                font: "monospace",
                backgroundColor: "rgba(0,0,0,0.5)",
                appearance: "mainwindow-statusbar",
                paddingTop: 5
            });
            var prefix = "/Status/";
            var label, cont, tx;
            var that = this;

            function vbar() {
                that.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(255,255,255,0.1)",
                    width: 1,
                    height: 1,
                    margin: [2,20,2,20]
                }));
            };

            // first column
            var cont = new qx.ui.container.Composite( new qx.ui.layout.VBox(0));
            label = new qapp.boundWidgets.RichLabel("","",prefix + "WCS");
            cont.add( label);
            this.add(cont);

            vbar();

            // second column
            var cont = new qx.ui.container.Composite( new qx.ui.layout.VBox(0));
            tx = function(val) {
                return val.replace(/ /g, "&nbsp;").replace(/\n/g, "<br>");
            };
            label = new qapp.boundWidgets.RichLabel("",tx,prefix + "Cursor");
            cont.add(label);
            this.add( cont);

            vbar();

            // third column
            var cont = new qx.ui.container.Composite( new qx.ui.layout.VBox(0));
            tx = function(val) {
                return val.replace(/ /g, "&nbsp;").replace(/\n/g, "<br>");
            };
            label = new qapp.boundWidgets.RichLabel("",tx,prefix + "Cursor2");
            cont.add(label);
            this.add(cont);

            vbar();

            // fourth column
            var cont = new qx.ui.container.Composite( new qx.ui.layout.VBox(0));
            cont.set( { allowGrowX: true });
            label = new qapp.boundWidgets.RichLabel("","",prefix + "Value");
            label.set( { width: 200, wrap: false, allowGrowX: true  });
            cont.add( label);
            this.add(cont);

            this.add( new qx.ui.core.Spacer, { flex: 1 });

            // fifth column
            var cont = new qx.ui.container.Composite( new qx.ui.layout.VBox(0));
            tx = function( v) {
                var res;
                if( typeof v === "string" && v.split(" ")[0] === "1") {
                    res = "Cursor is frozen.";
                } else {
                    res = "Cursor is not frozen.";
                }
                res += "<br/>Toggle with space bar.";
                return res;
            };
            label = new qapp.boundWidgets.RichLabel("",tx,prefix + "frozenCursor");
            cont.add( label);
            this.add(cont);

            // watch inside/outside cursor status and set opacity based on that
            tx = function(val) {
                that.setOpacity( val === "0" ? 1.0 : 0.5);
            }
            this.m_insideOutsideVar = fv.makeGlobalVariable( prefix + "isOutside", tx, false);
        },

        members: {
            m_insideOutsideVar: null
        }

    });

