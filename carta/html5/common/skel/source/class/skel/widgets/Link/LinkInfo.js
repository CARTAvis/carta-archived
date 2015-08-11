/**
 * Encapsulation of link information for a window with regard to a source
 * window.
 */

qx.Class.define("skel.widgets.Link.LinkInfo", {
    extend : qx.core.Object,

    construct : function() {

    },
    members : {
        // Links are directional; whether or not this window is the originator.
        source : false,
        // Location in pixels where a link should be marked.
        locationX : 0,
        locationY : 0,
        // Whether or not a link is supported to the source.
        linkable : false,
        // Whether this window is currently linked to the source.
        linked : false,
        // Identifier for this window.
        winId : "",
        //Type of plugin being displayed
        pluginId : "",
        //Location identifier.
        locationId : "",
        // Whether or not a bidirectional link is supported ( from this window
        // to source )
        twoWay : false

    }
});