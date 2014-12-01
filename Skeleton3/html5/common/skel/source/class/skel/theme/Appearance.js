/* ************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

qx.Theme.define( "skel.theme.Appearance", {

    extend: qx.theme.modern.Appearance,

    appearances: {


        "window/captionbar" :
        {
            style : function(states)
            {
                return {
                    decorator : (states.winsel ? "window-captionbar-active" :
                        "window-captionbar-inactive"),
                    textColor : states.winsel ? "window-caption-active-text" : "text-gray",
                    minHeight : 20,
                    paddingRight : 2
                };
            }
        },




        "splitpane": {
            style: function( states )
            {
                return {
                    decorator: "no-border"
                };
            }
        },

        "splitpane/splitter": {
            style: function( states )
            {
                return {
                    width          : states.horizontal ? 5 : undefined,
                    height         : states.vertical ? 5 : undefined,
                    backgroundColor: "background"
                };
            }
        },

        "display-main": {
            style: function( states )
            {
                return {
                    backgroundColor: "background"
                };
            }
        },

        "invisible-button": {
            style: function( states )
            {
                return {
                    //background : "transparent",
                    decorator: "no-border"
                };
            }
        },


        "popup-dialog": {
            style: function( states )
            {
                return {

                    padding        : 20,
                    margin         : [ 20, 20, 20, 20 ],
                    backgroundColor: "dialogBackground",
                    decorator      : "light-border"

                };
            }
        },

        "internal-area": {
            style: function( states )
            {
                return {

                    padding        : 2,
                    margin         : [ 2, 2, 2, 2 ],
                    backgroundColor: "dialogBackground",
                    decorator      : "line-border"

                };
            }
        },

        "status-bar": {
            style: function()
            {

                return {
                    decorator: "status-bar"
                };
            }
        }
    }
} );