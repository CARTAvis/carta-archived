qx.Theme.define("qapp.theme.Font",
    {
        extend: qx.theme.modern.Font,

        fonts: ( function() {
            var incr = 0;
            var lineHeight = 1.4;
            var tahoma = qx.core.Environment.get("os.name") == "osx" ?
                [ "Lucida Grande" ] :
                ((qx.core.Environment.get("os.name") == "win" &&
                    (qx.core.Environment.get("os.version") == "7" ||
                        qx.core.Environment.get("os.version") == "vista"))) ?
                    [ "Segoe UI", "Candara" ] :
                    [ "Tahoma", "Liberation Sans", "Arial", "sans-serif" ];
            var consolas = qx.core.Environment.get("os.name") == "osx" ?
                [ "Lucida Console", "Monaco" ] :
                ((qx.core.Environment.get("os.name") == "win" &&
                    (qx.core.Environment.get("os.version") == "7" ||
                        qx.core.Environment.get("os.version") == "vista"))) ?
                    [ "Consolas" ] :
                    [ "monospace", "Consolas", "DejaVu Sans Mono", "Courier New" ];
            if( qx.core.Environment.get("os.name") == "android")
                consolas = [ "monospace"];
            return {
                "default": {
                    size: 10 + incr,
                    lineHeight: lineHeight,
                    family: tahoma
                },

                "bold": {
                    size: 10 + incr,
                    lineHeight: lineHeight,
                    family: tahoma,
                    bold: true
                },

                "small": {
                    size: 9 + incr,
                    lineHeight: lineHeight,
                    family: tahoma
                },

                "bsmall": {
                    size: 9 + incr,
                    lineHeight: lineHeight,
                    family: tahoma,
                    bold: true
                },

                "blarge": {
                    size: 11 + incr,
                    lineHeight: lineHeight,
                    family: tahoma,
                    bold: true
                },


                "monospace": {
                    size: 10 + incr,
                    lineHeight: lineHeight,
                    family: consolas
                },

                "faint": {
                    italic: true,
                    color: "#ff0000",
                    size: 9 + incr,
                    lineHeight: lineHeight,
                    family: tahoma
                },

                "splashText": {
                    italic: false,
                    size: 15 + incr,
                    lineHeight: lineHeight,
                    family: tahoma
                }
            };
        })()
    });