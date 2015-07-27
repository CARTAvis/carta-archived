/**
 * Paths for states.
 */

qx.Class.define("skel.widgets.Path", {
    extend : qx.core.Object,
    type : "singleton",

    construct : function() {
        this.BASE_PATH = this.SEP + this.CARTA + this.SEP;
        this.ANIMATOR_TYPES = this.BASE_PATH +"AnimationTypes";
        this.CHANNEL_UNITS = this.BASE_PATH + "ChannelUnits";
        this.CLIPS = this.BASE_PATH + "Clips";
        this.COLORMAPS = this.BASE_PATH + "Colormaps";
        this.COORDINATE_SYSTEMS = this.BASE_PATH + "CoordinateSystems";
        this.DATA_COUNT = this.BASE_PATH + "controller"+ this.SEP + "dataCount";
        this.ERROR_HANDLER = this.BASE_PATH + "ErrorManager";
        this.FONTS = this.BASE_PATH + "Fonts";
        this.LAYOUT = this.BASE_PATH + "Layout";
        this.LAYOUT_PLUGIN = this.LAYOUT  + this.SEP + "plugins";
        this.MOUSE_X = this.BASE_PATH + this.MOUSE + this.SEP + "x" + this.SEP;
        this.MOUSE_Y = this.BASE_PATH + this.MOUSE + this.SEP + "y" + this.SEP;
        this.PREFERENCES = this.BASE_PATH + "Preferences";
        this.PREFERENCES_SAVE = this.BASE_PATH + "PreferencesSave";
        this.REGION = this.BASE_PATH + "region" + this.SEP;
        this.SETTINGS = this.BASE_PATH + "Settings";
        this.SNAPSHOTS = this.BASE_PATH + "Snapshots";
        this.THEMES = this.BASE_PATH + "Themes";
        this.TRANSFORMS_DATA = this.BASE_PATH +"TransformsData";
        this.TRANSFORMS_IMAGE = this.BASE_PATH + "TransformsImage";
    },
    
    statics : {
        CHANGE_VALUE : "changeValue",
        HORIZONTAL : "horizontal",
        VERTICAL : "vertical",
        MAX_RGB : 255
    },
    
    members : {
        ANIMATOR : "Animator",
        ANIMATOR_TYPES : "",
        AUTO_CLIP : "setAutoClip",
        BASE_PATH : "",
        CARTA : "CartaObjects",
        CASA_LOADER : "CasaImageLoader",
        CHANNEL_UNITS : "",
        CENTER : "center",
        CLIP_VALUE : "setClipValue",
        CLIPS : "",
        CLOSE_IMAGE : "closeImage",
        COLORMAP_PLUGIN : "Colormap",
        COLORMAPS : "",
        COORDINATE_SYSTEMS : "",
        DATA_COUNT : "",
        DATA_LOADER : "DataLoader",
        ERROR_HANDLER : "",
        FONTS : "",
        GRID_PLUGIN : "GridControls",
        HIDDEN : "Hidden",
        HISTOGRAM_PLUGIN : "Histogram",
        LAYOUT : "",
        LAYOUT_MANAGER : "Layout",
        LAYOUT_PLUGIN : "",
        MOUSE : "mouse",
        MOUSE_DOWN : "mouseDown",
        MOUSE_DOWN_SHIFT : "mouseDownShift",
        MOUSE_UP : "mouseUp",
        MOUSE_UP_SHIFT : "mouseUpShift",
        MOUSE_X : "",
        MOUSE_Y : "",
        PLUGINS : "ViewPlugins",
        PREFERENCES : "",
        PREFERENCES_SAVE : "",
        REGION : "",
        SEP : "/",
        SEP_COMMAND : ":",
        SETTINGS : "",
        SNAPSHOTS : "",
        STATE_LAYOUT : "Layout",
        STATE_SESSION : "Session",
        STATE_PREFERENCES : "Preferences",
        STATISTICS : "Statistics",
        THEMES : "",
        TRANSFORMS_DATA : "",
        TRANSFORMS_IMAGE : "",
        ZOOM : "zoom",
        VIEW : "view",
        VIEW_MANAGER : "ViewManager",
        
       
        /**
         * Returns the command for announcing the creation of a shape.
         * @param winId {String} the server side id of the object displaying the shape.
         * @return {String} command for registering the shape.
         */
        getCommandRegisterShape : function(winId){
            return winId + this.SEP_COMMAND +"registerShape";
        },
        
        /**
         * Returns the command for obtaining the unique identifier of a top level object.
         * @return {String} command for obtaining the unique identifier of a top level object.
         */
        getCommandRegisterView : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "registerView";
        },
        
        /**
         * Returns the command for getting a list of available data to load.
         * @return {String} command for getting a list of available data to load.
         */
        getCommandLoadData : function(){
            return this.BASE_PATH + this.DATA_LOADER + this.SEP_COMMAND + "getData";
        },
            
        /**
         * Returns the command for loading selected data.
         * @return {String} command for loading selected data.
         */
        getCommandDataLoaded : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "dataLoaded";
        },
        
        /**
         * Returns the command for changing a window location.
         * @return {String} command for changing a window location.
         */
        getCommandMoveWindow : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "moveWindow";
        },
        
        /**
         * Returns the command for indicating which object the animator should control.
         * @return {String} command for linking the animator with a controlled object.
         */
        getCommandLinkAnimator : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "linkAnimator";
        },
        
        /**
         * Returns the command for resetting (emptying) the layout.
         * @return {String} command for resetting the layout.
         */
        getCommandClearLayout : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "clearLayout";
        },
        
        /**
         * Notify the server that state updates are needed.
         */
        getCommandRefreshState : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "refreshState";
        },
        
        
        
        /**
         * Returns the command to register snapshots.
         * @return {String} command for registering snapshots.
         */
        getCommandRegisterSnapshots : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "registerSnapshots";
        },
        
        /**
         * Returns the command for resizing the number of rows/columns in the layout.
         * @return {String} command for resizing the layout.
         */
        getCommandSetLayoutSize : function(){
            return this.BASE_PATH + this.LAYOUT_MANAGER + this.SEP_COMMAND + "setLayoutSize";
        },
        
        /**
         * Returns the command for updating the plug-in that is displayed.
         * @return {String} command for updating the displayed plug-in.
         */
        getCommandSetPlugin : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "setPlugin";
        },
        
        /**
         * Returns the command for updating a shape.
         * @return {String} command for updating a shape.
         */
        getCommandShapeChanged : function(shapeId){
            return shapeId + this.SEP_COMMAND +"shapeChanged";
        }


    }
});