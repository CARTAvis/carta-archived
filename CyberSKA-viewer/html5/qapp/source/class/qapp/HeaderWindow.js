/**
 * Author: Pavol Federl
 * Date: 08/04/13
 * Time: 1:46 PM
 *
 * This widget displays the contents of the FITS header..
 */

/* global qx, fv, qapp */
/* jshint expr: true */

/**
 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.makeGlobalVariable)
*/

qx.Class.define("qapp.HeaderWindow",
    {
        extend: qapp.BetterWindow,

        construct:function ( hub)
        {
            this.base(arguments, hub);

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setWidth( 520);
            this.setHeight( 500);

            this.setLayout(new qx.ui.layout.Canvas());

            this.m_textArea = new qx.ui.form.TextArea();
            this.m_textArea.setReadOnly( true);
            this.m_textArea.setAppearance( "code-textarea");
            this.m_textArea.setSelectable( false);
            this.add( this.m_textArea, { left:"0%", right: "0%", top: "0%", bottom: "0%"});

            this.setCaption("Header");
            this.setContentPadding(0, 0, 0, 0);

            // make a shared variable to listen to header content changes
            var that = this;
            fv.makeGlobalVariable( "/FITSHeader", function(val) {
                that.m_textArea.setValue( val || "");
            }, false);

//            fv.console.log("HeaderWindow constructed.");
        },

        members:{

        },

        properties:{

        }

    });

