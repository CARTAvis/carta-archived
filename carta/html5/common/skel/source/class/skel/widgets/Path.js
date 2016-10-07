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
        this.CONTOUR_GENERATE_MODES = this.BASE_PATH + "ContourGenerateModes";
        this.CONTOUR_LINE_STYLES = this.BASE_PATH + "ContourStyles";
        this.CONTOUR_SPACING_MODES = this.BASE_PATH + "ContourSpacingModes";
        this.COORDINATE_SYSTEMS = this.BASE_PATH + "CoordinateSystems";
        this.DATA_COUNT = this.BASE_PATH + "controller"+ this.SEP + "dataCount";
        this.ERROR_HANDLER = this.BASE_PATH + "ErrorManager";
        this.FONTS = this.BASE_PATH + "Fonts";
        this.GAMMA = this.BASE_PATH + "Gamma";
        this.INTENSITY_UNITS = this.BASE_PATH + "UnitsIntensity";
        this.LABEL_FORMATS = this.BASE_PATH + "LabelFormats";
        this.LAYER_COMPOSITION_MODES = this.BASE_PATH + "LayerCompositionModes";
        this.LAYOUT = this.BASE_PATH + "Layout";
        this.LAYOUT_PLUGIN = this.LAYOUT  + this.SEP + "plugins";
        this.LINE_STYLES = this.BASE_PATH + "LineStyles";
        this.MOUSE_X = this.BASE_PATH + this.MOUSE + this.SEP + "x" + this.SEP;
        this.MOUSE_Y = this.BASE_PATH + this.MOUSE + this.SEP + "y" + this.SEP;
        this.PLOT_STYLES = this.BASE_PATH + "ProfilePlotStyles";
        this.PREFERENCES = this.BASE_PATH + "Preferences";
        this.PREFERENCES_SAVE = this.BASE_PATH + "PreferencesSave";
        this.PROFILE_FREQ_UNITS = this.BASE_PATH + "UnitsFrequency";
        this.PROFILE_WAVE_UNITS = this.BASE_PATH + "UnitsWavelength";
        this.PROFILE_GEN_MODES = this.BASE_PATH + "GenerateModes";
        this.PROFILE_STATISTICS = this.BASE_PATH + "ProfileStatistics";
        this.REGION = this.BASE_PATH + this.REGION_DATA + this.SEP;
        this.REGION_TYPES = this.BASE_PATH + "RegionTypes";
        this.SETTINGS = this.BASE_PATH + "Settings";
        this.SNAPSHOTS = this.BASE_PATH + "Snapshots";
        this.SPECTRAL_UNITS = this.BASE_PATH + "UnitsSpectral";
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
        //CENTER : "center",
        CLIP_VALUE : "setClipValue",
        CLIPS : "",
        CLOSE_IMAGE : "closeImage",
        CLOSE_REGION : "closeRegion",
        COLORMAP_PLUGIN : "Colormap",
        COLORMAPS : "",
        CONTOUR_GENERATE_MODES : "",
        CONTOUR_LINE_STYLES : "",
        CONTOUR_SPACING_MODES : "",
        COORDINATE_SYSTEMS : "",
        DATA : "data",
        DATA_COUNT : "",
        DATA_LOADER : "DataLoader",
        ERROR_HANDLER : "",
        FIT : "fit",
        FONTS : "",
        GAMMA : "",
        GRID_PLUGIN : "GridControls",
        HIDDEN : "Hidden",
        HIDE_IMAGE : "hideImage",
        HISTOGRAM_PLUGIN : "Histogram",
        IMAGE_CONTEXT : "ImageContext",
        IMAGE_ZOOM : "ImageZoom",
        IMAGE_DATA : "image",
        INPUT_EVENT : "inputEvent",
        INTENSITY_UNITS : "",
        LABEL_FORMATS : "",
        LAYER_COMPOSITION_MODES : "",
        LAYOUT : "",
        LAYOUT_MANAGER : "Layout",
        LAYOUT_PLUGIN : "",
        LINE_STYLES : "",
        MOUSE : "mouse",
        MOUSE_DOWN : "mouseDown",
        MOUSE_DOWN_SHIFT : "mouseDownShift",
        MOUSE_UP : "mouseUp",
        MOUSE_UP_SHIFT : "mouseUpShift",
        MOUSE_X : "",
        MOUSE_Y : "",
        PLUGINS : "ViewPlugins",
        PLOT_STYLES : "",
        PREFERENCES : "",
        PREFERENCES_SAVE : "",
        PROFILE : "Profiler",
        REGION : "",
        REGION_DATA : "region",
        REGION_TYPES : "",
        SEP : "/",
        SEP_COMMAND : ":",
        SETTINGS : "",
        SHOW_IMAGE : "showImage",
        SNAPSHOTS : "",
        SPECTRAL_UNITS : "",
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