/* ************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

qx.Theme.define( "skel.theme.Decoration", {
    extend: qx.theme.modern.Decoration,

    decorations: {

        "light-border": {
            style: {
                width : 5,
                radius: 3,
                color : "#000000"
            }
        },

        "line-border": {
            style: {
                width : 2,
                radius: 1,
                color : "#000000"
            }
        },

        "no-border": {
            style: {
                width: 0

            }
        },

        "status-bar": {
            style: {
                backgroundColor: "background"
            }
        },

        "popup-dialog": {
            style: {
                backgroundColor: "background"
            }
        },

        "desktop": {
            style: {
                backgroundColor: "background"
            }
        },

//        // window default
//        "win": {
//            style: {
//                backgroundColor: "background",
//                width: 5,
////                widthTop          : 5,
//                color          : "selection2"
//            }
//        },
//
//        // currently 'selected' window
//        "window-pane-active": {
//            style: {
//                backgroundColor: "background",
//                width: 5,
////                widthTop          : 5,
////                colorTop          : "selection"
//                color          : "selection"
//            }
//        },

        "window" :
        {
            style : {
                radius : [0, 0, 0, 0],
                shadowBlurRadius : 4,
                shadowLength : 2,
                shadowColor : "shadow"
            }
        },

        "window-resize-frame" :
        {
            style : {
                radius : [0, 0, 0, 0],
                width : 1,
                color : "border-main"
            }
        },

        "window-resize-frame-incl-statusbar" :
        {
            include : "window-resize-frame",
            style : {
                radius : [0, 0, 0, 0]
            }
        },

        "window-pane" :
        {
            style :
            {
                backgroundColor : "background-pane",
                width : 0,
                color : "window-border",
                widthTop : 0
            }
        },

        "window-captionbar-active" :
        {
            style : {
                width : 0,
                color : "window-border",
                colorBottom : "window-border-caption",
                radius : [0, 0, 0, 0],
                gradientStart : ["window-caption-active-start", 30],
                gradientEnd : ["window-caption-active-end", 70]
            }
        },

        "window-captionbar-inactive" :
        {
            include : "window-captionbar-active",
            style : {
                radius : [0, 0, 0, 0],
                gradientStart : ["window-caption-inactive-start", 30],
                gradientEnd : ["window-caption-inactive-end", 70]
            }
        }


    }
} );