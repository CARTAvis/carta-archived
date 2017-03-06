/* ************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

qx.Theme.define( "skel.theme.Color", {
    extend: qx.theme.modern.Color,

    colors: {
        "text"            : "#000000", //black
        "background"      : "#DBDBDB", //gray
        "border"          : "#CCCC99", //light yellow-green
        "selection"       : "#6699CC", //blue danube
        //"selection"       : "#FF0000",
        //"selection2"      : "#66CCB8",
        "warning"         : "#FF9900", //orange peel
        "dialogBackground": "#E0E0E0", //rock
        "error": "#FF0000",
        "blank"           : "#FFFFFF", //white
        //To get lighter convert each color part to a decimal and add a percentage c + c * .1
        //For darker c - c * .1
        //Based on blue danub
        "window-caption-active-start" : "#70A8E0",
        "window-caption-active-end" : "#578AB8"
    }
} );
