/* ***************************************************************************************
 * Created by pfederl on Feb 9, 2015
 *
 * ***************************************************************************************/

/* global qx, mImport */
/* jshint strict: false */

/**
 *
 * This is a simple extension of the raw skel.boundWidgets.View.ViewWithInputDivSuffixed, where we append a suffix
 * to the end of the view name. The suffix is usually '/view', but it is determined
 * by calling an external singleton, i.e.:
 *
 * suffix = skel.widgets.Path.getInstance().SEP + skel.widgets.Path.getInstance().VIEW
 *
 */
qx.Class.define( "skel.boundWidgets.View.ViewWithInputDivSuffixed", {

    extend: skel.boundWidgets.View.ViewWithInputDiv,

    /**
     * @param partialViewName {String} partial name of the view.
     */
    construct: function( partialViewName )
    {
        var path = skel.widgets.Path.getInstance();
        var suffix = path.SEP + path.VIEW; // e.g. "/view"

        this.base( arguments, partialViewName + suffix );
    }

} );
