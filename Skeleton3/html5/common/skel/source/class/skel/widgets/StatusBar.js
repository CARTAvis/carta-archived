/**
 * Manages a text area for displaying status messages and a tool bar for iconifying
 * windows.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.StatusBar",
    {
        extend: qx.ui.core.Widget,
        implement: skel.widgets.IHidable,
        include: skel.widgets.MShowHideMixin,

        construct: function () {
            this.base(arguments);
            
            this._setLayout(new qx.ui.layout.HBox(5));
            	
            //Status Message
        	this.m_statusMessage = new qx.ui.basic.Label( "Status Text" );
        	this.m_statusMessage.setAllowGrowX( true );
        	this._add( this.m_statusMessage, {flex:1});
        
        	//Tool bar for holding iconified windows.
        	this.m_iconifiedWindows = new qx.ui.toolbar.ToolBar();
        	this.m_iconifiedWindows.setAllowGrowX( true );
        	this._add( this.m_iconifiedWindows);
        	
        	this.setZIndex( 1000 );
        	
        	//Initially hide the status bar.
        	this.show( this, false );
       
        },
        
        members: {
        	
        	/**
        	 * Removes the button representing an iconfied window from the status bar.
        	 * @param removeButton {qx.ui.toolbar.MenuButton} the button to remove.
        	 */
        	 _removeIconifiedWindow : function( removeButton ){
             	if ( removeButton != null ){
             		this.m_iconifiedWindows.remove( removeButton );
             	}
             },
             
             /**
              * Adds a button to the status bar for restoring a minimized window.
              * @param ev {qx.event.type.Data} the data identifying the window being minimized.
              * @param restoreListener {skel.Application} the listener to be notified when the minimized
              * 	window needs to be restored to its original location.
              */
             addIconifiedWindow : function( ev, restoreListener ){
            	var data = ev.getData();
             	var menuButton = new qx.ui.toolbar.MenuButton("Restore: " +data["title"]);
             	menuButton.setAlignX("right");
             	menuButton.setShowArrow( false );
             	this.m_iconifiedWindows.add( menuButton );
             	menuButton.addListener("execute", function () {
                     restoreListener.restoreWindow( data['row'], data['col'] );
                 }, restoreListener);
             	menuButton.addListener( "execute", function(){
             		this._removeIconifiedWindow( menuButton );
             	}, this);
             	
             },
             
             /**
              * Set whether or not the status bar should always be visible or whether
              * it should be shown/hidden based on mouse location and/or status change events.
              * @param alwaysVisible {boolean} true if the status window should always
              * 	be visible; false otherwise.
              */
             setStatusAlwaysVisible : function( alwaysVisible ){
            	 this.setAlwaysVisible( this, alwaysVisible );
             },
           
          
         	
             /*
              * Hides or shows the status bar based on the location of the mouse.
              * Mouse close to screen bottom = show; Otherwise, hide.
              * @param ev the mouse event.
              */
             showHideStatus : function( ev ){
            	var widgetLoc = skel.widgets.Util.getTop( this );
            	var mouseLoc = ev.getDocumentTop();
            	this.showHide( this, mouseLoc, widgetLoc );
            	 
             },
             
             animateSize : function( percent, show ){
            	 if ( show ){
            		 var newSize = Math.round( percent * this.m_animationSize);
        			 this.setHeight( newSize );
            	 }
            	 else { 
            		 var newSize = Math.round(( 1 - percent ) * this.m_animationSize);	
    				 this.setHeight( newSize );
            	 }
             },
             
             animateEnd : function( show ){
            	 if ( show ){
            		 this.setHeight( this.m_animationSize );
			 	}
			 	else {
			 		this.setHeight( 0 );
			 	}
        	},
             
             removeWidgets : function(){
            	 if ( this.hasLayoutChildren() ){
            		 this._remove( this.m_statusMessage );
            		 this._remove( this.m_iconifiedWindows );
            	 }
             },
             
             addWidgets : function(){
            	 if ( !this.hasLayoutChildren() ){
            		 this._add( this.m_statusMessage, {flex:1});
            		 this._add( this.m_iconifiedWindows );
            	 }
             },
             
             getAnimationHeight : function(){
            	 return this.m_animationSize;
             },
                     	
             m_iconifiedWindows: null,
             m_statusMessage: null
           
        },
        
       

        properties: {
            appearance: {
                refine: true,
                init: "status-bar"
            }

        }

    });

