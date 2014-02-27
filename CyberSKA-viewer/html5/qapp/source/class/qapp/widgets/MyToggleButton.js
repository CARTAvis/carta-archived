/**
 * This file is part of CyberSKA Image Viewer (www.cyberska.org)
 * Author: Pavol Feder ( federl@gmail.com )
 * Date: 25/11/13
 * Time: 7:56 PM
 *
 * My toggle button class. By default it has an icon that is either in on or off state. User
 * can supply his/her own icons as well.
 */

/* global qx, qapp, fv */
/* jshint expr: true */

/**
 @ignore(fv.*)

 @asset(qapp/icons/toggle-off-8.png)
 @asset(qapp/icons/toggle-on-8.png)

 */


qx.Class.define( "qapp.widgets.MyToggleButton",
    {
        extend: qx.ui.form.ToggleButton,

        /**
         * Constructor
         * @param labelPrefix {String}
         * @param labelPostfix {String|Function}
         * @param varPath {String} Path to the bound shared variable
         *
         * If no icons are specified, the default ones are used.
         * If only one icon is specified, it is used whether the toggle is on or off.
         * If both icons are specified, first one is used when toggle is off, second one when the
         * toggle is on.
         */
        construct: function ( label, icon1, icon2 ) {
            var defaultOnIcon = "qapp/icons/toggle-on-8.png";
            var defaultOffIcon = "qapp/icons/toggle-off-8.png";

            if ( icon1 == null ) {
                this.m_icon1 = defaultOnIcon;
                this.m_icon2 = defaultOffIcon;
            }
            else if( icon2 == null) {
                this.m_icon1 = icon1;
                this.m_icon2 = icon1;
            }
            else {
                this.m_icon1 = icon2;
                this.m_icon2 = icon1;
            }
            this.base( arguments, label, this.m_icon2 );

            // if we have both icons defined, toggle between with value changed event
            if ( this.m_icon1 != this.m_icon2 ) {
                this.addListener( "changeValue", this._changeValueCB, this );
            }
        },

        members: {

            _changeValueCB: function ( val ) {
                this.setIcon( val.getData() ? this.m_icon1 : this.m_icon2 );
            }
        }

    } );

