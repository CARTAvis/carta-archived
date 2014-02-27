qx.Theme.define("qapp.theme2m.Font",
    {
        extend: qx.theme.modern.Font,

        fonts: ( function() {
            var incr = 2;
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
                    [ "Consolas", "DejaVu Sans Mono", "Courier New", "monospace" ];
            if( qx.core.Environment.get("os.name") == "android")
                consolas = [ "monospace"];
            return {
                "default": {
                    size: 10 + incr,
                    lineHeight: 1.4,
                    family: tahoma
                },

                "bold": {
                    size: 10 + incr,
                    lineHeight: 1.4,
                    family: tahoma,
                    bold: true
                },

                "small": {
                    size: 9 + incr,
                    lineHeight: 1.4,
                    family: tahoma
                },

                "bsmall": {
                    size: 9 + incr,
                    lineHeight: 1.4,
                    family: tahoma,
                    bold: true
                },

                "blarge": {
                    size: 11 + incr,
                    lineHeight: 1.4,
                    family: tahoma,
                    bold: true
                },


                "monospace": {
                    size: 10 + incr,
                    lineHeight: 1.4,
                    family: consolas
                },

                "faint": {
                    italic: true,
                    color: "#ff0000",
                    size: 9 + incr,
                    lineHeight: 1.4,
                    family: tahoma
                },

                "splashText": {
                    italic: false,
                    size: 15 + incr,
                    lineHeight: 1.4,
                    family: tahoma
                }
            };
        })()
    });