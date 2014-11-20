/**
 * Paths for states.
 */

qx.Class.define("skel.widgets.Path", {
    extend : qx.core.Object,
    type : "singleton",

    construct : function() {
        this.BASE_PATH = this.SEP + this.CARTA + this.SEP;
        this.DATA_COUNT = this.BASE_PATH + "controller"+ this.SEP + "dataCount";
        this.LAYOUT = this.BASE_PATH + "Layout";
        this.LAYOUT_PLUGIN = this.LAYOUT  + this.SEP + "plugins";
        this.MOUSE_X = this.BASE_PATH + this.MOUSE + this.SEP + "x" + this.SEP,
                this.MOUSE_Y = this.BASE_PATH + this.MOUSE + this.SEP + "y"
                        + this.SEP
        this.REGION = this.BASE_PATH + "region" + this.SEP;
        this.ZOOM = this.BASE_PATH + "zoom" + this.SEP;
    },
    members : {
        ANIMATOR : "Animator",
        AUTO_CLIP : "setAutoClip",
        BASE_PATH : "",
        CARTA : "CartaObjects",
        CASA_LOADER : "CasaImageLoader",
        CLIP_VALUE : "setClipValue",
        COLORMAP : "Colormap",
        DATA_COUNT : "",
        DATA_LOADER : "DataLoader",
        HIDDEN : "Hidden",
        LAYOUT : "",
        LAYOUT_MANAGER : "Layout",
        LAYOUT_PLUGIN : "",
        MOUSE : "mouse",
        MOUSE_X : "",
        MOUSE_Y : "",
        PLUGINS : "ViewPlugins",
        REGION : "",
        SEP : "/",
        SEP_COMMAND : ":",
        ZOOM : "",
        VIEW : "view",
        VIEW_MANAGER : "ViewManager",
        
        getCommandRegisterShape : function(winId){
            return winId + this.SEP_COMMAND +"registerShape";
        },
        
        getCommandRegisterView : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "registerView";
        },
        
        
        
        getCommandLoadData : function(){
            return this.BASE_PATH + this.DATA_LOADER + this.SEP_COMMAND + "getData";
        },
            
        getCommandDataLoaded : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "dataLoaded";
        },
        
        getCommandLinkAnimator : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "linkAnimator";
        },
        
        getCommandClearLayout : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "clearLayout";
        },
        
        getCommandRestoreState : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "restoreState";
        },
        
        getCommandSetLayoutSize : function(){
            return this.BASE_PATH + this.LAYOUT_MANAGER + this.SEP_COMMAND + "setLayoutSize";
        },
        
        getCommandSetPlugin : function(){
            return this.BASE_PATH + this.LAYOUT_MANAGER + this.SEP_COMMAND + "setPlugin";
        },
        
        getCommandShapeChanged : function(shapeId){
            return shapeId + this.SEP_COMMAND +"shapeChanged";
        },
        
        getCommandSaveState : function(){
            return this.BASE_PATH + this.VIEW_MANAGER + this.SEP_COMMAND + "saveState";
        }
        
 


    }
});