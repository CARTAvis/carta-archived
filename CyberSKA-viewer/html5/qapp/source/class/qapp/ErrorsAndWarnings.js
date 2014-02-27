/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 08/04/13
 * Time: 1:46 PM
 * To change this template use File | Settings | File Templates.
 */

/**

 @ignore(fv.console.log)
 @ignore(fv.assert)

 ************************************************************************ */

/* global qx */
/* global qapp */

qx.Class.define('qapp.ErrorsAndWarnings',
    {
        extend:qapp.BetterWindow,

        construct:function ( hub)
        {
            this.base(arguments, hub);

            // subscribe to warnings and errors
            this.m_hub.subscribe( "gs.warning.changed", this._warningCB, this);
            this.m_hub.subscribe( "gs.error.changed", this._errorCB, this);

            // create the UI

            this.setCaption( "Server says:");
            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setModal( true);
            this.setLayout(new qx.ui.layout.Canvas());
            this.setContentPadding(5);
            this.setMinWidth( 200);
            this.setMinHeight( 100);
            this.m_label = new qx.ui.basic.Label();
            this.m_label.setSelectable( false);
            this.m_label.setRich( true);
            this.m_label.setValue("");
            this.m_label.set({ allowStretchX: true});
            this.add( this.m_label, { left:"0%", right: "0%", top: "0%", bottom: "0%"});

            this.addListener( "close", function() {
                this.m_label.setValue( "");
            }, this);
        },

        members:{

            _warningCB: function(val) {
                var txt = this.m_label.getValue();
                txt += "<div style='background:pink;margin-top:5px;font-size: 15px;font-weight: bold;'>";
                txt += "Warning</div>";
                txt += val.text;
                this.m_label.setValue( txt);
                this.setHeight( null);
                this.setWidth( null);
                this.open();
                this.center();
            },

            _errorCB: function(val) {
                var txt = this.m_label.getValue();
                txt += "<div style='background:red;color:white;margin-top:5px;font-size: 15px;font-weight: bold;'>";
                txt += "Error</div>";
                txt += val.text;
                this.m_label.setValue( txt);
                this.setHeight( null);
                this.setWidth( null);
                this.open();
                this.center();
            }
        }
    });

