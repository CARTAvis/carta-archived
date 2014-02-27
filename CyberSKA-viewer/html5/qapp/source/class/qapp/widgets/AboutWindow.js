/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 15/11/13
 * Time: 6:56 PM
 *
 * To change this template use File | Settings | File Templates.
 */

/* global qx, qapp, fvVersion */
/* jshint expr: true */

/**
 @ignore(fv.*)
 @ignore(fvVersion)
 */

qx.Class.define("qapp.widgets.AboutWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base( arguments, hub);

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setWidth( 370);
            this.setHeight( 240);
            this.setCaption( "About");
            this.setContentPadding(0);
            this.setAlwaysOnTop( true);

            this.setLayout(new qx.ui.layout.Grow);

            /* language="HTML" */
            var txt = "<div style=\"background:black;color:white;font-family:\'Lucida Grande\',Verdana,sans-serif;padding:5px;\">\n\n<span style=\'color: #3F7895;font-size:30px;\'>CYBER</span>\n<span style=\'color: #E4F1DB; font-weight: bold;font-size:30px;\'>SKA</span>\n<span style=\'font-size:24px;color:#fff075;\'>Image Viewer</span>\n<p style=\'font-size:1em;color:#aaa;margin:0px;\'>Version $version</p>\n</div>\n\n<div style=\'padding:5px;\'>\n    <p style=\'font-size:1.3em;\'>You can find documentation at the following address:<br/>\n        <a target=\'_blank\' href=\'http://www.cyberska.org/pg/file/read/32459\'>http://www.cyberska.org/pg/file/read/32459</a>\n    </p>\n    <div style=\"height:30px;\"></div>\n    <img src=\'http://www.cyberska.org/mod/profile/icondirect.php?lastcache=1271887182&joindate=1268672046&guid=243&size=small\'\n         style=\'vertical-align: text-top;padding-right: 5px;\' align=\"left\"/>\n    <span>Written by Pavol Federl<br/>\n    <a target=\'_blank\' href=\'mailto:federl@gmail.com?subject=About%20the%20CyberSKA%20Image%20Viewer\'>federl@gmail.com</a>\n    </span>\n</div>\n";
            txt = txt.replace(/\$version/g, fvVersion);

            var html = new qx.ui.embed.Html();
            html.setSelectable( false);
            html.setHtml( txt);
            this.add( html);

/*
            var scrollContainer = new qx.ui.container.Scroll();
            scrollContainer.set({
                width: 300,
                height: 200
            });
//            scrollContainer.setLayout( new qx.ui.layout.Grow);
            this.add( scrollContainer);

            var frame = new qx.ui.embed.Iframe("http://www.cyberska.org/pg/pages/view/32432/cyberska-image-viewer-documentation");
            scrollContainer.add( frame);

            */

            // center on screen
            this.addListenerOnce("resize", this.center, this);
        },

        members: {

        }
    });

