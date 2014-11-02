/**
 * A display window specialized for controlling data animators.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 
 ******************************************************************************/

qx.Class
        .define(
                "skel.widgets.DisplayWindowAnimation",
                {
                    extend : skel.widgets.DisplayWindow,

                    /**
                     * Constructor.
                     */
                    construct : function(row, col, index ) {
                        this.base(arguments, "animator", row, col, index );
                        this.setLayout(new qx.ui.layout.VBox(5));
                        this.m_links = [];
                    },

                    members : {
                        /**
                         * Send one or more commands to the server to update it on what animators are on
                         * display.
                         */
                        _addRemoveAnimators : function(){
                            var animObj = JSON.parse( this.m_sharedVar.get() );
                            for ( var i = 0; i < this.m_checks.length; i++ ){
                                var checkName = this.m_supportedAnimations[i];
                                var selected = this.m_checks[i].getValue();
                                var alreadyThere = false;
                                if ( animObj.animators[checkName] ){
                                    alreadyThere = true;
                                }
                                
                                //Decide if the animator's visibility has changed with respect to what the serve thinks.
                                var changedValue = false;
                                if ( ( !alreadyThere && selected) || (alreadyThere && !selected) ){
                                   changedValue = true;
                                }
                                
                                //If the visibility of the animator has changed issue a command to tell the server to sync up.
                                if ( changedValue ){
                                   var addRemVal = "addAnimator";
                                   if ( !selected ){
                                       addRemVal = "removeAnimator";
                                   }
                                   var path = skel.widgets.Path.getInstance();
                                   var cmd = this.m_identifier + path.SEP_COMMAND + addRemVal;
                                   var params = "type:"+this.m_checks[i].getLabel();
                                   this.m_connector.sendCommand( cmd, params, function(){});
                                }
                            }
                        },
                        
                        
                        /**
                         * Callback for a state change; update the animators that are displayed.
                         */
                        _animationCB : function( val ){
                            if ( this.m_sharedVar ){
                                var animObj = JSON.parse( this.m_sharedVar.get() );
                                //Go through the supported animations.  If it is in the list, mark
                                //as visible; otherwise mark as invisible.
                                for ( var j = 0; j < this.m_checks.length; j++ ){
                                    var visible = false;
                                    var checkName = this.m_checks[j].getLabel();
                                    if ( animObj.animators[checkName]){
                                        visible = true;
                                    }
                                    
                                    if ( this.m_checks[j].getValue() != visible ){
                                        this.m_checks[j].setValue( visible );
                                    }
                                }
                                this._showHideAnimation();
                            }
                        },
                        
                        /**
                         * Returns animation specific menu buttons.
                         */
                        getWindowSubMenu : function() {
                            var windowMenuList = []

                            var animationButton = new qx.ui.toolbar.MenuButton("Animation");
                            animationButton.setMenu(this._initShowMenuWindow());
                            windowMenuList.push(animationButton);
                            return windowMenuList;
                        },

                        /**
                         * Initializes animator specific items for the context
                         * menu.
                         */
                        _initDisplaySpecific : function() {
                            this.m_showAnimationButton = new qx.ui.menu.Button("Animate");
                            this.m_showAnimationButton.setMenu(this._initShowMenu());
                            this.m_contextMenu.add(this.m_showAnimationButton);
                        },
                        
                        /**
                         * Add a callback to the main window shared variable to deal with animation
                         * related changes.
                         */
                        _initSharedVarAnim : function(){
                            this.m_sharedVar.addCB( this._animationCB.bind( this ));
                            this._animationCB( this.m_sharedVar.get());
                        },
                        
                        

                        /**
                         * Initializes a menu for the different animators that
                         * can be displayed that is suitable for the main menu;
                         * corresponding check boxes are coordinated with the
                         * context menu.
                         */
                        _initShowMenuWindow : function() {
                            var showMenu = new qx.ui.menu.Menu;
                            for (var i = 0; i < this.m_supportedAnimations.length; i++) {
                                var animId = this.m_supportedAnimations[i];
                                var animCheck = new qx.ui.menu.CheckBox(animId);
                                animCheck.setValue( this.m_checks[i].getValue());
                                showMenu.add(animCheck);
                                this.m_checks[i].bind("value", animCheck,
                                        "value");
                                animCheck.bind( "value", this.m_checks[i], "value");
                            }
                            return showMenu;
                        },
                        
                        /**
                         * Initialize the context menu check boxes that indicate which animators
                         * are displayed.
                         */
                        _initChecks : function(){
                            if ( this.m_checks == null ){
                                this.m_checks = [];
                            
                                for (var i = 0; i < this.m_supportedAnimations.length; i++) {
                                    var animId = this.m_supportedAnimations[i];
                                    if ( this.m_checks.length <= i ){
                                        var check = new qx.ui.menu.CheckBox( animId);
                                        check.setValue( false );
                                        this.m_checks.push( check );
                                        this.m_checks[i].listener = this.m_checks[i].addListener("changeValue", this._addRemoveAnimators, this);
                                    }
                                }
                            }
                        },

                        /**
                         * Initializes the context menu for the different animators that can
                         * be displayed.
                         */
                        _initShowMenu : function() {
                            var showMenu = new qx.ui.menu.Menu;
                            this._initChecks();
                            for ( var i = 0; i < this.m_checks.length; i++ ){
                                showMenu.add(this.m_checks[i]);
                            }
                            return showMenu;
                        },
                        

                        
                        /**
                         * Adds or removes a specific animator from the display
                         * based on what the user has selected from the menu.
                         */
                        _showHideAnimation : function() {
                            if ( this.m_animators == null ){
                                this.m_animators = {}
                            }
                            this._initChecks(); 
                            for (var i = 0; i < this.m_checks.length; i++) {

                                var animId = this.m_checks[i].getLabel();
                                var animVisible = this.m_checks[i].getValue();
                                if (animVisible) {
                                    if (this.m_animators[animId] == undefined ) {
                                        this.m_animators[animId] = new skel.boundWidgets.Animator(animId, this.m_identifier);
                                    }
                                      
                                    if (this.m_content.indexOf(this.m_animators[animId]) < 0) {
                                        this.m_content.add(this.m_animators[animId]);
                                    }
                                } 
                                else {
                                    if (this.m_content.indexOf(this.m_animators[animId]) >= 0) {
                                        this.m_content.remove(this.m_animators[animId]);
                                    }
                                }
                            }
                        },
                        
                        /**
                         * Implemented to remove the title.
                         */
                        windowIdInitialized : function() {
                            arguments.callee.base.apply(this, arguments );
                            this._initDisplaySpecific();
                            this.m_content.remove(this.m_title);
                            this._initSharedVarAnim();
                        },
                        
                        
                        m_showAnimationButton : null,
                        m_supportedAnimations : [ "Channel", "Image", "Region" ],
                        m_checks : null,
                        m_animators : null
                    }

                });
