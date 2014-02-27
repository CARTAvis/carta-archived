/**

 set(qx/decoration/Modern/toolbar/toolbar-part.gif)

 */


qx.Theme.define("qapp.theme.Decoration",
    {
        extend: qx.theme.modern.Decoration,

        decorations: {
            "mainwindow-statusbar": {
                decorator: qx.ui.decoration.Single,
                style: {
                    widthTop: 1,
                    color: "border-separator"
                }
            },

            "tooltip" :
            {
                style :
                {
                    width : 1,
                    color : "#000000",
                    radius: [0, 5, 5, 5],
                    shadowColor : "shadow",
                    shadowBlurRadius : 5,
                    shadowLength : 1,

                    startColor: "#EDE500",
                    endColor: "#ffffff",
                    startColorPosition: 25,
                    endColorPosition: 100
                }
            },

            "group2" :
            {
                include: "group",
                style : {
                    backgroundColor : "rgba(255,255,255,0.9)",
                    radius : 4,
                    color : "rgba(255,255,255,0.4)",
                    width: 2
                }
            },

            "group3" :
            {
                include: "group",
                style : {
                    backgroundColor : "rgba(255,255,255,0.9)",
                    radius : 4,
                    color : "rgba(255,255,255,0.7)",
                    width: 0
                }
            },

            "toolbar-part" :
            {
                style :
                {
                    backgroundImage  : "qx/decoration/Modern/toolbar/toolbar-part.gif",
                    backgroundRepeat : "repeat-y"
                }
            },

            "light-border" :
            {
                style :
                {
                    width : 1,
                    radius: 3,
                    color : "rgba(0,0,0,0.2)"
                }
            },

            "no-border" :
            {
                style :
                {
                    width : 1,
                    radius: 3,
                    color : "rgba(0,0,0,0)"
                }
            },

/*
            "red-knob" :
            {
                include: "scrollbar-slider-horizontal",
                style : {
                    gradientStart : ["#ff0000", 0],
                    gradientEnd: ["#aa0000", 100]
                }
            },

            "green-knob" :
            {
                include: "scrollbar-slider-horizontal",
                style : {
                    gradientStart : ["#00ff00", 0],
                    gradientEnd: ["#00aa00", 100]
                }
            },

            "blue-knob" :
            {
                include: "scrollbar-slider-horizontal",
                style : {
                    gradientStart : ["#0000ff", 0],
                    gradientEnd: ["#0000aa", 100]
                }
            },
*/

            // decorators for text fields that look little more like labels
            "input-tl" :
            {
                include: "input",
                style :
                {
                    color : "#888",
                    innerColor : "border-inner-input",
                    innerWidth: 0,
                    width : 1,
                    style: "dotted",
                    backgroundColor : "background-light",
                    startColor : "input-start",
                    endColor : "input-end",
                    startColorPosition : 0,
                    endColorPosition : 12,
                    colorPositionUnit : "px",
                    radius: 5
                }
            },

            "border-invalid-tl" : {
                include : "input-tl",
                style : {
                    color : "border-invalid"
                }
            },

            "input-focused-tl" : {
                include : "input-tl",
                style : {
                    startColor : "#DDE100",
                    endColor : "#DDE100"
//                    endColorPosition : 4
                }
            },

            "input-focused-invalid-tl" : {
                include : "input-focused-tl",
                style : {
                    innerColor : "input-focused-inner-invalid",
                    color : "border-invalid"
                }
            },

            "input-disabled-tl" : {
                include : "input-tl",
                style : {
                    color: "input-border-disabled"
                }
            }

        }
    });