/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 05/04/13
 * Time: 4:27 PM
 * To change this template use File | Settings | File Templates.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 @asset(qapp/icons/pin12.png)
 @asset(qapp/icons/pin12off.png)

 ************************************************************************ */


qx.Class.define("qapp.BetterWindow",
    {
        extend: qx.ui.window.Window,

        construct: function (hub) {
            this.base(arguments);

            if( hub == null)
                this.m_hub = null;
            else
                this.m_hub = hub;

            var toolLayout = new qx.ui.layout.HBox(3);
            toolLayout.setReversed(true);
            toolLayout.setAlignY("middle");
            this.m_extraHBox = new qx.ui.container.Composite(toolLayout);

            this.m_pinToggle = this.addToolButton( null, "qapp/icons/pin12off.png" )
                .set( {
                    show: "icon",
                    toolTipText: "Pin window to the top"
                } );
            this.m_pinToggle.addListener( "click", function () {
                this.toggleAlwaysOnTop();
                if ( this.getAlwaysOnTop() ) {
                    this.getLayoutParent().getWindowManager().bringToFront( this );
                }
                this.getLayoutParent().getWindowManager().updateStack();
            }, this );

            this.addListener( "changeAlwaysOnTop", function ( e ) {
                if ( this.getAlwaysOnTop() ) {
//                    this.m_pinToggle.setOpacity( 1 );
                    this.m_pinToggle.setIcon( "qapp/icons/pin12.png")
                } else {
//                    this.m_pinToggle.setOpacity( 0.8 );
                    this.m_pinToggle.setIcon( "qapp/icons/pin12off.png")
                }
            }, this );

//            this.m_pinToggle = this.addToolToggle( null, "qapp/icons/pin12off.png", "qapp/icons/pin12.png" )
//                .set( {
//                    show: "icon",
//                    toolTipText: "Pin window to the top."
//                } );
//            this.m_pinToggle.addListener( "changeValue", function (e) {
//                this.setAlwaysOnTop( e.getData());
//                if ( this.getAlwaysOnTop() ) {
//                    this.getLayoutParent().getWindowManager().bringToFront( this );
//                }
//                this.getLayoutParent().getWindowManager().updateStack();
//            }, this );
//            this.addListener( "changeAlwaysOnTop", function ( e ) {
//                this.m_pinToggle.setValue( this.getAlwaysOnTop() );
//            }, this );

            // add touch support for moving the window
            if( qx.core.Environment.get("event.touch")) {
                var fake = function(target, finger) {
                    var nativeEvent = {};

                    nativeEvent.button = 0; // for left button
                    nativeEvent.wheelDelta = 0;
                    nativeEvent.wheelDeltaX = 0;
                    nativeEvent.wheelDeltaY = 0;
                    nativeEvent.wheelX = 0;
                    nativeEvent.wheelY = 0;
                    nativeEvent.target = target;

                    nativeEvent.clientX = finger.clientX;
                    nativeEvent.clientY = finger.clientY;
                    nativeEvent.pageX = finger.pageX;
                    nativeEvent.pageY = finger.pageY;
                    nativeEvent.screenX = finger.screenX;
                    nativeEvent.screenY = finger.screenY;

                    nativeEvent.shiftKey = false;
                    nativeEvent.ctrlKey = false;
                    nativeEvent.altKey = false;
                    nativeEvent.metaKey = false;

                    return nativeEvent;
                };
                var faken = function(target, finger, type) {
                    var evt = fake(target, finger);
                    var mouseEvent = type == "mousewheel" ?
                        new qx.event.type.MouseWheel() :
                        new qx.event.type.Mouse();
                    mouseEvent.init(evt, target, null, true, true);
                    mouseEvent.setType(type);
                    return mouseEvent;
                };

                var capture = true;
                var cb = this.getChildControl("captionbar");
                var title = this.getChildControl("title");
                function fingerOverTitle( finger) {
                    var box2 = title.getContentLocation("box");
                    fv.console.log( box2.left, box2.right, finger.pageX);
                    if( finger.pageX < box2.left) return false;
                    if( finger.pageX > box2.right) return false;
                    return true;
                }
                cb.addListener( "touchstart", function(e){
                    var finger = e.getAllTouches()[0];
                    if( ! fingerOverTitle( finger)) {
                        return;
                    }
                    var mev = faken( e.getTarget(), finger, "mousedown");
                    this._onMoveMouseDown( mev);
                    e.preventDefault();
                }, this, capture);
                cb.addListener( "touchend", function(e){
                    var finger = e.getChangedTargetTouches()[0];
                    if( ! fingerOverTitle( finger)) {
                        return;
                    }
                    var finger = e.getChangedTargetTouches()[0];
                    var mev = faken( e.getTarget(), finger, "mouseup");
                    this._onMoveMouseUp( mev);
                    e.preventDefault();
                }, this, capture);
                cb.addListener( "touchmove", function(e){
                    var finger = e.getChangedTargetTouches()[0];
                    var mev = faken( e.getTarget(), finger, "mousemove");
                    this._onMoveMouseMove( mev);
                    e.preventDefault();
                }, this, capture);
                cb.addListener( "tap", function(e){
                    var finger = e.getChangedTargetTouches()[0];
                    if( ! fingerOverTitle( finger)) {
                        return;
                    }
                    e.preventDefault();
                }, this, true);
                cb.addListener( "longtap", function(e){
                    e.preventDefault();
                }, this, true);
            }
        },


        members: {

            setLayoutRequestId: function (id) {
                fv.assert(this.m_hub !== null, "hub is NULL");
                this.m_layoutId = id;
                this.m_hub.subscribe("toui.windowMove", function (val) {
                    if (val.id !== this.m_layoutId) return;
                    if (this.isMaximized()) this.restore();
                    this.moveTo(val.left, val.top);
                    if (val.width !== null) this.setWidth(val.width);
                    if (val.height !== null) this.setHeight(val.height);
                }, this);
            },

            addToolButton: function (label, icon) {
                icon = icon || null;
                var button = new qx.ui.form.Button( label, icon);
                button.setAllowGrowY(false);
                button.setFocusable(false);
                this.m_extraHBox.add(button);
                if (label == null)
                    button.setAppearance("better-window-tool-button-icon");
                else
                    button.setAppearance("better-window-tool-button");
                return button;
            },

            /**
             * Create a toggle button in the title bar on the right.
             *
             * @param label {String} optional label (either label or icon must be specified)
             * @param icon {String} optional icon to be used when not not toggled or both
             * @param icon2 {String} optional 2nd icon to be used when toggled
             * @returns {qx.ui.form.ToggleButton}
             */
            addToolToggle: function ( label, icon, icon2 ) {
                icon = icon || null;
                icon2 = icon2 || null;
//                var button = new qx.ui.form.ToggleButton( label, icon );
                var button = new qapp.widgets.MyToggleButton( label, icon );
                button.setAllowGrowY( false );
                button.setFocusable( false );
                button.setAlignY( "middle" );
                this.m_extraHBox.add( button );
                button.setAppearance( "better-window-tool-toggle-button" );
                if( icon2 != null) {
                    button.addListener( "changeValue", function(e){
                        button.setIcon( e.getData() ? icon : icon2);
                    });
                }
                return button;
            },

            /**
             * toggles visibility of the window
             */
            toggle: function () {
                if ( this.isVisible() ) {
                    this.close();
                }
                else {
                    this.open();
                    this._positionMeToBeVisible();
                }
            },

            _positionMeToBeVisible: function () {
                var parent = this.getLayoutParent();
                if ( ! parent ) return;

                var myBounds = this.getBounds();
                if ( ! myBounds ) return;
                var top = myBounds.top, left = myBounds.left;

                var pBounds = parent.getBounds();
                if ( ! pBounds ) return;

                var hint = this.getSizeHint();
                if ( ! hint ) return;
                var width = hint.width, height = hint.height;

                if ( left + width > pBounds.left + pBounds.width )
                    left = pBounds.left + pBounds.width - width;

                if ( top + height > pBounds.top + pBounds.height )
                    top = pBounds.top + pBounds.height - height;

                if ( left < pBounds.left )
                    left = pBounds.left;

                if ( top < pBounds.top )
                    top = pBounds.top;

                if ( left < 0 ) left = 0;
                if ( top < 0 ) top = 0;

                this.moveTo( left, top );
            },

            // overridden
            _createChildControlImpl: function (id, hash) {
                var control;

                switch (id) {
                    case "title":
                        control = new qx.ui.basic.Label(this.getCaption());
                        control.setWidth(0);
                        control.setAllowGrowX(true);

                        var layout = new qx.ui.layout.HBox();
                        var hbox = new qx.ui.container.Composite(layout);
                        hbox.add(control, {flex: 1});
                        hbox.add(this.m_extraHBox, { flex: 0});
                        hbox.add(new qx.ui.core.Spacer(10, 1), { flex: 0});

                        this.getChildControl("captionbar").add(hbox, {row: 0, column: 1});
                        break;
                }

                return control || this.base(arguments, id);
            },

            _emit: function (path, data) {
                fv.assert(this.m_hub !== null, "hub is NULL");
                this.m_hub.emit(path, data);
            },

            m_nCustomButtons: 0,
            m_hub: null,
            m_extraHBox: null

        },

        properties: {
            appearance: {
                refine: true,
                init: "better-window"
            }

        }

    });

