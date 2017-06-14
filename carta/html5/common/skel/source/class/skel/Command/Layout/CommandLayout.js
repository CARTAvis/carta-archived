/**
 * Container for commands that change the layout.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Layout.CommandLayout", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Layout", null );
        this.m_cmds = [];
        this.m_cmds.push(skel.Command.Layout.CommandLayoutDefault.getInstance());
        this.m_cmds.push(skel.Command.Layout.CommandLayoutAnalysis.getInstance());
        this.m_cmds.push(skel.Command.Layout.CommandLayoutHistogramAnalysis.getInstance());
        this.m_cmds.push(skel.Command.Layout.CommandLayoutImage.getInstance());
        this.m_cmds.push(skel.Command.Layout.CommandLayoutCustom.getInstance());
        this.setValue( this.m_cmds );

        //Listen to the layout so if a different one is selected on the server we can update the GUI.
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector.getSharedVar( pathDict.LAYOUT );
        this.m_sharedVar.addCB( this._layoutChangedCB.bind(this));
    },

    members : {

        _layoutChangedCB : function(){
            var layoutObjJSON = this.m_sharedVar.get();
            if ( layoutObjJSON ){
                try {
                    var layout = JSON.parse( layoutObjJSON );
                    var type = layout.layoutType;
                    if (this.setCheckedType(type) == false){
                        console.log( "CommandLayout: unrecognized layout type: "+type);
                    }
                }
                catch( err ){
                    console.log( "CommandLayout, could not parse: "+layoutObjJSON );
                }
            }
        },

        setupAllCheckStatus: function(cmdLayout){
            var length = this.m_cmds.length;
            for (var i=0; i<length; i++){
                var storeCmdLayout = this.m_cmds[i];
                if (storeCmdLayout != cmdLayout) {
                    storeCmdLayout.setValue( false );
                } else {
                    storeCmdLayout.setValue( true );
                }
            }
        },

        /**
         * Set the children of this command active/inactive.
         * @param active {boolean} true if the command should be active; false otherwise.
         */
        //Written so that when a window is added or removed, the server-side update of
        //the value will not trigger a re-layout.
        setActive : function( active ){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setActive( active );
            }
        },

        // no use, 20170307
        setValues : function( image, analysis, custom ){
            this.setActive( false );
            var imageCmd = skel.Command.Layout.CommandLayoutImage.getInstance();
            imageCmd.setValue( image );
            var analysisCmd = skel.Command.Layout.CommandLayoutAnalysis.getInstance();
            analysisCmd.setValue( analysis );
            var customCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
            customCmd.setValue( custom );
            this.setActive( true );
        },

        setCheckedType: function(layoutType) {
          if(layoutType) {
              var cmdLayout = null;
              if (layoutType == "ImageComposite") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutImage.getInstance();
              } else if (layoutType == "Image") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutImage.getInstance();
              } else if (layoutType == "Analysis") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutAnalysis.getInstance();
              } else if (layoutType == "HistogramAnalysis") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutHistogramAnalysis.getInstance();
              } else if (layoutType == "Custom") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutCustom.getInstance();
              } else if (layoutType == "Default") {
                  cmdLayout  = skel.Command.Layout.CommandLayoutDefault.getInstance();
              }

              if (cmdLayout){
                  // important, should use setActive(false) & setActive(true),
                  // otherwise will trigger loop layout-reset
                  this.setActive( false );
                  this.setupAllCheckStatus(cmdLayout);
                //   cmdLayout.setValue( true );
                  this.setActive( true );

                  return true;
              }
              return false;
          }

          return false;

        },

        m_sharedVar : null
    }
});
