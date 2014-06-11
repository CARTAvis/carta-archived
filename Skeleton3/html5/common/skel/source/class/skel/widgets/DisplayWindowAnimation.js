/**
 * A display window specialized for controlling data animators.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayWindowAnimation",
    {
        extend: skel.widgets.DisplayWindow,
        
        /**
         * Constructor.
         */
        construct: function ( /*hub*/) {
            this.base(arguments);
            this.setLayout( new qx.ui.layout.VBox(5));
           
        },

        members: {
        	
        	/**
        	 * Implemented to remove the title.
        	 */
        	setPlugin : function( label ){
        		this._initDisplaySpecific();
        		arguments.callee.base.apply(this, arguments);
        		this.remove(this.m_title);
        	},
        	
        	/**
        	 * Adds or removes a specific animator from the display based on what the
        	 * user has selected from the menu.
        	 */
        	_showHideAnimation : function(){
        		for ( var i = 0; i < this.m_checks.length; i++ ){
        			
        			var animId = this.m_checks[i].getLabel();
        			var animVisible = this.m_checks[i].getValue();
        			if ( animVisible ){
        				if ( this.m_animators[animId] == null ){
        					this.m_animators[animId] = new skel.widgets.Animator(animId);
        				}
        				if ( this.indexOf( this.m_animators[animId]) == -1 ){
        					this.add( this.m_animators[animId] );
        				}
        			}
        			else {
        				if ( this.indexOf( this.m_animators[animId]) >= 0 ){
        					this.remove( this.m_animators[animId]);
        				}
        			}
        		}
        	},
        	
        	/**
        	 * Returns animation specific menu buttons.
        	 */
        	getWindowMenu: function(){
        		var windowMenuList = []
        		var dataButton = new qx.ui.toolbar.MenuButton( "Data");
        		dataButton.setMenu( this._initDataMenu() );
        		windowMenuList.push( dataButton );
        		
        		var animationButton = new qx.ui.toolbar.MenuButton( "Animation");
        		animationButton.setMenu( this._initShowMenuWindow());
        		windowMenuList.push( animationButton );
        		return windowMenuList;
        	},
       
            _initDisplaySpecific: function(){
            	//Data
        		this.m_dataButton = new qx.ui.menu.Button( "Data");
        		this.m_dataButton.setMenu( this._initDataMenu() );
        		this.m_contextMenu.add( this.m_dataButton );
        		
        		//Animate
        		this.m_showAnimationButton = new qx.ui.menu.Button( "Animate");
        		this.m_showAnimationButton.setMenu( this._initShowMenu() );
        		this.m_contextMenu.add( this.m_showAnimationButton );
            },
            
            _initDataMenu : function(){
            	var detachDataButton = new qx.ui.menu.Button( "Detach...");
        		var attachDataButton = new qx.ui.menu.Button( "Attach...");
        		var manageDataButton = new qx.ui.menu.Button( "Manage...");
        		var dataMenu = new qx.ui.menu.Menu();
        		dataMenu.add( manageDataButton );
        		dataMenu.add( attachDataButton );
        		dataMenu.add( detachDataButton );
        		return dataMenu;
            },
            
            /**
             * Initializes a menu for the different animators that can be displayed
             * that is suitable for the main menu; corresponding check boxes are
             * coordinated with the context menu.
             */
            _initShowMenuWindow : function(){
        		var showMenu = new qx.ui.menu.Menu;
        		for ( var i = 0; i < this.m_supportedAnimations.length; i++ ){
        			var animId = this.m_supportedAnimations[i];
        			var animCheck = new qx.ui.menu.CheckBox( animId );
        			showMenu.add( animCheck );
        			this.m_checks[i].bind( "value", animCheck, "value");
        			animCheck.bind( "value", this.m_checks[i], "value");
        		}
        		return showMenu;
            },
             
           /**
            * Initializes the context menu for the different animators that can
            * be displayed.
            */
        	_initShowMenu : function(){
        		var showMenu = new qx.ui.menu.Menu;
        		for ( var i = 0; i < this.m_supportedAnimations.length; i++ ){
        			var animId = this.m_supportedAnimations[i];
        			this.m_checks[i] = new qx.ui.menu.CheckBox( animId );
        			showMenu.add( this.m_checks[i] );
        			this.m_checks[i].addListener( "changeValue", function(ev){
        				this._showHideAnimation();
        			}, this);
        			if ( i == 0 ){
        				this.m_checks[i].setValue( true );
        			}
            		
        		}
        		return showMenu;
            },
             

            _emit: function (path, data) {
                //fv.assert(this.m_hub !== null, "hub is NULL");
                //this.m_hub.emit(path, data);
            },

         
            m_hub: null,
    	    m_showAnimationButton: null, 	
    		m_dataButton: null,
    		m_supportedAnimations : ["Channel", "Image", "Region"],
    		m_checks: [],
    		m_animators: {}
        }

       
    });

