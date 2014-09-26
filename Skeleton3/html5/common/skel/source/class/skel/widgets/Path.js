/**
 * Paths for states.
 */

qx.Class.define("skel.widgets.Path", {
    extend : qx.core.Object,
    type : "singleton",

    construct : function() {
        this.BASE_PATH = this.SEP + this.CARTA + this.SEP;
        this.AUTO_CLIP = this.SEP + this.CARTA + this.SEP + "autoClip";
        this.CLIP_VALUE = this.SEP + this.CARTA + this.SEP + "clipValue";
        this.DATA_COUNT = this.BASE_PATH + "controller"+ this.SEP + "dataCount";
        this.DATA_LOADED = "dataLoaded";
        this.FILE_SYSTEM = this.BASE_PATH + "data";
        this.LAYOUT = this.BASE_PATH + "layout";
        this.LAYOUT_ROWS = this.LAYOUT + this.SEP + "rows";
        this.LAYOUT_COLS = this.LAYOUT + this.SEP + "cols";
        this.LAYOUT_PLUGIN = this.LAYOUT  + this.SEP + "plugins";
        this.MOUSE_X = this.BASE_PATH + this.MOUSE + this.SEP + "x" + this.SEP,
                this.MOUSE_Y = this.BASE_PATH + this.MOUSE + this.SEP + "y"
                        + this.SEP
        this.REGION = this.BASE_PATH + "region" + this.SEP;
        this.ZOOM = this.BASE_PATH + "zoom" + this.SEP;
        this.PLUGIN_LIST = this.BASE_PATH + "pluginList";
        this.PLUGIN_LIST_STAMP = this.PLUGIN_LIST + this.SEP + "stamp";
        this.PLUGIN_LIST_NAME = this.PLUGIN_LIST + this.SEP + "name";
    },
    members : {
        BASE_PATH : "",
        AUTO_CLIP : "",
        CARTA : "carta",
        CASA_LOADER : "CasaImageLoader",
        CLIP_VALUE : "",
        DATA_COUNT : "",
        DATA_LOADED : "",
        FILE_SYSTEM : "",
        FRAME_STEP : "frameStep",
        LAYOUT : "",
        LAYOUT_ROWS : "",
        LAYOUT_COLS : "",
        LAYOUT_PLUGIN : "",
        MOUSE : "mouse",
        MOUSE_X : "",
        MOUSE_Y : "",
        REGION : "",
        SEP : "/",
        ZOOM : "",
        LINK_ANIMATOR : "linkAnimator",
        PLUGIN_LIST : "",
        PLUGIN_LIST_STAMP : "",
        PLUGIN_LIST_NAME : ""

    }
});