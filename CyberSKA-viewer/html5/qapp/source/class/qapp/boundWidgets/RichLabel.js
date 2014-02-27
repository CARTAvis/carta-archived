/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 11/11/13
 * Time: 7:04 PM
 *
 * RichLabel is same as Label, but with rich flags set to true (i.e. capable of rendering html)
 */

/* global qx, qapp, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)
 */


qx.Class.define("qapp.boundWidgets.RichLabel",
    {
        extend: qapp.boundWidgets.Label,

        /**
         * Constructor
         * @param labelPrefix {String}
         * @param labelPostfix {String|Function}
         * @param varPath {String} Path to the bound shared variable
         */
        construct: function ( labelPrefix, labelPostfix, varPath) {
            this.base(arguments, labelPrefix, labelPostfix, varPath);
            this.setRich( true);
        }

    });

