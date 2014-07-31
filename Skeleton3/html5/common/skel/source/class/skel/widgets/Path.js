/**
 * Paths for states.
 */

qx.Class.define("skel.widgets.Path",
 {
	extend: qx.core.Object,
	type: "singleton",
	
	construct : function(){
		 this.BASE_PATH = this.SEP + this.CARTA + this.SEP;
		 this.DATA_LOADED = "dataLoaded";
		 this.FILE_SYSTEM = this.BASE_PATH + "data";
		 this.MOUSE_X = this.BASE_PATH + this.MOUSE + this.SEP + "x" + this.SEP,
		 this.MOUSE_Y = this.BASE_PATH + this.MOUSE + this.SEP + "y" + this.SEP
		 this.REGION = this.BASE_PATH + "region" + this.SEP;
		 this.ZOOM = this.BASE_PATH + "zoom" + this.SEP;
		 this.PLUGIN_LIST = this.BASE_PATH + "pluginList";
		 this.PLUGIN_LIST_STAMP = this.PLUGIN_LIST + this.SEP + "stamp";
		 this.PLUGIN_LIST_NAME = this.PLUGIN_LIST + this.SEP + "name";
	},
   members:{
	 BASE_PATH : "",
	 CARTA : "carta",
	 DATA_LOADED : "",
	 FILE_SYSTEM : "",
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