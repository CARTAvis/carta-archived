/**
 * Purpose of this class:
 *
 * To sync profile windows
 *
 */

/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.lib.closure)
 @ignore(fv.GLOBAL_DEBUG)

 */

qx.Class.define(
    "qapp.ProfileController",
    {
        extend: qx.core.Object,

        /**
         * @param hub {Hub} The hub on which it will communicate
         * @param root {Widget}where to add windows (this is basically a IWindowManager)
         * @param boundsWidget {Widget} invisible widget for applying layouts
         */
        construct: function (hub) {

            this.base( arguments);
            this.m_hub = hub;

            this.m_g1dfitters = {};

            // create all profile windows
            this.m_windows = {
                ix: new qapp.widgets.ProfileWindow( this.m_hub, "ix", true),
                iy: new qapp.widgets.ProfileWindow( this.m_hub, "iy", true),
                iz: new qapp.widgets.ProfileWindow( this.m_hub, "iz", true),
                qz: new qapp.widgets.ProfileWindow( this.m_hub, "qz", true),
                uz: new qapp.widgets.ProfileWindow( this.m_hub, "uz", true),
                pi: new qapp.widgets.ProfileWindow( this.m_hub, "pi", true),
                pa: new qapp.widgets.ProfileWindow( this.m_hub, "pa", true),
                qu: new qapp.widgets.QUProfileWindow( this.m_hub, "1"),
                "rs-nan": new qapp.widgets.ProfileWindow( this.m_hub, "rs-nan", true),
                "rs-min": new qapp.widgets.ProfileWindow( this.m_hub, "rs-min", true),
                "rs-max": new qapp.widgets.ProfileWindow( this.m_hub, "rs-max", true),
                "rs-avg": new qapp.widgets.ProfileWindow( this.m_hub, "rs-avg", true),
                "rs-sum": new qapp.widgets.ProfileWindow( this.m_hub, "rs-sum", true),
                "rs-std": new qapp.widgets.ProfileWindow( this.m_hub, "rs-std", true),
                "rs-bkg": new qapp.widgets.ProfileWindow( this.m_hub, "rs-bkg", true),
                "rs-sumbkg": new qapp.widgets.ProfileWindow( this.m_hub, "rs-sumbkg", true),
                "rs-maxbkg": new qapp.widgets.ProfileWindow( this.m_hub, "rs-maxbkg", true)
            };

            // listen for UI actions on the profiles and emit corresponding messages
            fv.lib.closure( function(){
                for( var id in this.m_windows) {
                    this._listenToUI( this.m_windows[id], id);
                }
            }, this);

            // listen for application state changes and redirect them to the appropriate profiles
            this.m_hub.subscribe( "toui.g1df.changed", this._profileStateCB, this);
//            this.m_hub.subscribe( "toui.quprofile.changed", this._quprofileStateCB, this);

            // listen to changes to the list of g1d fitters
            this.m_hub.subscribe( "toui.g1dFittersList.changed", this._g1dFittersListCB, this);
        },

        members: {

            getProfileWindow: function ( name) {
                return this.m_windows[ name];
            },

            _listenToUI: function ( win, id) {
                var w = win.getProfileWidget();

                win.addListener( "invokeFit", this._invokeFitCB.bind(this, id));

                w.addListener( "uiAction", function(e) {
                    var val = e.getData();
                    this._emit( "ui.g1df.uiAction", val);
                }, this);
            },

            _invokeFitCB:function( id) {
                this._emit( "ui.g1df.invokeG1Dfit", { id: id });
            },

            _profileStateCB: function ( val) {
                // every basic profile has a window/widget associated with it, so just
                // set the data
                if( this.m_windows.hasOwnProperty( val.name)) {
                    this.m_windows[ val.name].getProfileWidget().setState( val.state);
//                    this.m_windows[ val.name].setCaption( val.state.title);
                }
                else if( this.m_g1dfitters.hasOwnProperty( val.name)) {
                    this.m_g1dfitters[ val.name].getProfileWidget().setState( val.state);
//                    this.m_g1dfitters[ val.name].setCaption( val.state.title);
                }
                else {
                    fv.console.error( "_profileStateCB does not know about " + val.name, val.state);
                }
            },

/*
            _quprofileStateCB: function ( val) {
                if( val.name !== "1") {
                    fv.console.error( "_quprofileStateCB does not know about " + val.name, val.state);
                    return;
                }
                this.m_windows.qu.getProfileWidget().setState( val.state);
                this.m_windows.qu.setCaption( val.state.title);
            },
*/

            _g1dFittersListCB: function( st) {
                fv.GLOBAL_DEBUG && fv.console.log( "g1dfitters = ", st);
                // for every active fitter on the list, create one if it does not exist already
                // for every inactive fitter on the list, hide it
                st.forEach( function( e, ind) {
                    var active = e;
                    var id = "g1d" + ind;
                    if( active) {
                        var win = this.m_g1dfitters[id];
                        if( win == null) {
                            fv.GLOBAL_DEBUG && fv.console.log( "creating g1d " + id);
                            win = new qapp.widgets.ProfileWindow( this.m_hub, id, false);
                            this.m_g1dfitters[id] = win;
                            this._listenToUI( win, id);
                            win.addListener( "close", this._closeG1DwindowCB.bind(this,id), this);
                        }
                        if( ! win.isVisible()) { win.toggle(); }
                    }
                    else {
                        if( this.m_g1dfitters[id] == null) return; // continue the loop
                        this.m_g1dfitters[id].close(); // careful, this will fire close event...
                    }
                }, this);
            },

            _closeG1DwindowCB: function( id) {
                this._emit( "ui.g1df.releaseG1Dfit", {
                    id: id
                });
            },

            _emit: function( path, data) {
                this.m_hub.emit( path, data);
            }

        }
    });
