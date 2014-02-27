
/**

 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.hub.emit)
 @ignore(fv.GLOBAL_DEBUG)

************************************************************************ */

qx.Class.define("qapp.widgets.FileShortcutsWindow",
    {
        extend:qapp.BetterWindow,

        construct:function ( hub)
        {
            this.base(arguments, hub);

//            this.m_hub = hub;

            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
//            this.setWidth( 520);
//            this.setHeight( 500);

            this.setLayout(new qx.ui.layout.VBox( 3));



            this.setCaption("File shortcuts...");
            this.setContentPadding(3);

            // TODO: there should be no parsedState reference at all, controller
            // should be handling this

            // subscribe to both parsedState, and set file shortcuts
            this.m_hub.subscribe( "toui.parsedState", function( parsedState) {
                fv.console.log("parsedState");
                this._updateShortcuts( parsedState.fileShortcuts);
            }, this);
            this.m_hub.subscribe( "toui.setFileShortcuts",
                this._updateShortcuts, this);

            // let the controller know we can receive parsed state updates
            this._emit( "ui.parsedStateRequest");

            fv.console.log("Shortcuts window constructed.");
        },

        members:{
/*
            m_hub: null,

            emit: function( path, data) {
                this.m_hub.emit( path, data);
            },

*/
            _updateShortcuts: function( shortcuts) {
                fv.GLOBAL_DEBUG && fv.console.log( "updating shortcuts %O", shortcuts );
                if( ! qx.lang.Type.isArray( shortcuts)) return;
                this.removeAll();
                shortcuts.forEach( function( name, index) {
                    var button = new qx.ui.form.Button( name);
                    button.addListener( "execute", function(){
                        fv.console.log( "load shortcut #" + index + " name=" + name);
                        this._emit( "ui.loadFileShortcut", index);
                    }, this);
                    this.add( button);
                }, this);
            }

        },

        properties:{

        }

    });

