/* ************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

/* globals qx */

qx.Theme.define( "skel.theme.Decoration", {
    extend: qx.theme.modern.Decoration,

    decorations: {

        "light-border": {
            style: {
                width : 6,
                radius: 3,
                color : "selection"
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
        
        "slider-red" :
        {
           style :
            {          
                backgroundColor : "#FF0000"
            }
         },
         
         "slider-blue" :
         {
            style :
             {          
                 backgroundColor : "#0000FF"
             }
          },
          
          "slider-green" :
          {
             style :
              {          
                  backgroundColor : "#00FF00"
              }
           },

        "desktop": {
            style: {
                backgroundColor: "background"
            }
        },
        
        "redBorder" : {
            include: "input-focused",
            style : {
                width : 2,
                color : "red"
            }    
        },



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
        
        "window-pane-selected" :
        {
            style :
            {
                backgroundColor : "background-pane",
                width : 2,
                color : "selection",
                widthTop : 2
            }
        }
    }
} );