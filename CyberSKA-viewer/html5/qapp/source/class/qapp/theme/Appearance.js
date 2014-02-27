/* global qx */

qx.Theme.define("qapp.theme.Appearance",
    {
        extend: qx.theme.modern.Appearance,

        appearances: {
            "better-window": {
                alias: "window",
                include: "window",
                style: function () {
                    return {
                    };
                }
            },

            "better-window-tool-button": {
                alias: "button",
                include: "button",
                style: function () {
                    return {
                        textColor: "#000000",
                        padding: [0, 5]
                    };
                }
            },

            "better-window-tool-toggle-button": {
                alias: "button",
                include: "button",
                style: function (states) {
                    return {
                        textColor: states.checked ? "#000000" : "#555555",
//                        font: states.checked ? "bsmall" : "small",
                        font: "bsmall",
                        padding: [1, 5]
                    };
                }
            },

            "better-window-tool-button-icon": {
                alias: "atom",
                style: function (states) {
                    return {
                        padding: 1,
                        decorator: states.hovered ? "light-border" : "no-border",
                        margin : [ 5, 0, 5, 0 ]
                    };
                }
            },

            "toolbar-menubutton": {
                alias: "atom",

                style: function (states) {
                    var decorator = (states.pressed || states.hovered) && !states.disabled ? "selected" : undefined;

                    return {
                        decorator: decorator,
                        textColor: states.pressed || states.hovered ? "text-selected" : undefined,
                        padding: [ 1, 3 ]
                    };
                }
            },

            "code-textarea": {
                include: "textarea",
                alias: "textarea",

                style: function () {
                    return {
                        padding: 2,
                        font: "monospace",
                        decorator : "mainwindow-statusbar"
                    };
                }
            },

            "mainwindow-statusbar": {
                include: "label",
                alias: "label",

                style: function () {
                    return {
                        decorator : "mainwindow-statusbar"
                    };
                }
            },

            "valueLabel": {
                include: "label",
                alias: "label",

                style: function () {
                    return {
                        allowGrowX : false,
                        textAlign : "left",
                        textColor : "#0000ff"
                    };
                }
            },

            "valueLabel2": {
                include: "label",
                alias: "label",

                style: function () {
                    return {
                        allowGrowX : true,
                        textAlign : "left",
                        textColor : "#000000",
                        font: "monospace"
                    };
                }

            },

            "cmap-preset-button": {
                include: "button",
                alias: "button",
                style: function() {
                    return {
                        font: "small",
                        padding: [2,2]
                    };
                }
            },

            "tooltip" :
            {
              include : "popup",

              style : function()
              {
                return {
                  backgroundColor : "background-tip",
                  padding : 8,
                  offset : [ 15, 5, 5, 5 ],
                  decorator: "tooltip"
                };
              }
            },

            "MainSideBar-button": {
                alias: "button",
                include: "button",
                style: function () {
                    return {
                        textColor: "#000000",
                        allowGrowX: true,
                        padding: [3, 3]
                    };
                }
            },

            "MainSideBar-label": {
                alias : "label",
                include : "label",
                style: function () {
                    return {
                        textColor: "#ffffff",
                        font: "blarge",
                        allowGrowX: true,
                        textAlign: "center"
                    };
                }
            },

            "MainSideBar": {
                style: function () {
                    return {
                        backgroundColor: "transparent"
                    };
                }
            },

            "mwMoreSettings": {
                include: "groupbox",
                alias: "groupbox",
                style : function()
                {
                    return {
                        decorator : "group2"
                    };
                }
            },

            "mwMoreSettings/legend" :
            {
                include: "groupbox/legend",
                alias : "atom",

                style : function()
                {
                    return {
                        textColor : "#ffffff"
                    };
                }
            },

            "mwMoreSettings/frame" :
            {
                include: "groupbox/frame",
                style : function()
                {
                    return {
                        decorator : "group3"
                    };
                }
            },

            // text fields that look little more like labels
            "textfieldLight" :
            {
                style : function(states)
                {
                    var decorator;

                    var focused = !!states.focused;
                    var invalid = !!states.invalid;
                    var disabled = !!states.disabled;

                    if (focused && invalid && !disabled) {
                        decorator = "input-focused-invalid";
                    } else if (focused && !invalid && !disabled) {
                        decorator = "input-focused";
                    } else if (disabled) {
                        decorator = "input-disabled";
                    } else if (!focused && invalid && !disabled) {
                        decorator = "border-invalid";
                    } else {
                        decorator = "input";
                    }
                    decorator += "-tl";

                    var textColor;
                    if (states.disabled) {
                        textColor = "text-disabled";
                    } else if (states.showingPlaceholder) {
                        textColor = "text-placeholder";
                    } else {
                        textColor = "text-input";
                    }

                    return {
                        decorator : decorator,
                        padding : [ 1, 1, 1, 1 ],
                        textColor : textColor
                    };
                }
            }
        }
    });