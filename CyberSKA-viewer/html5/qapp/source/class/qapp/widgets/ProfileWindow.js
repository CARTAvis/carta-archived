/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 08/04/13
 * Time: 1:46 PM
 * To change this template use File | Settings | File Templates.
 */

/**

 @asset(qapp/icons/swheel12.png)

 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.console.*)
 @ignore(fv.makeGlobalVariable)

 ************************************************************************ */

qx.Class.define("qapp.widgets.ProfileWindow",
    {
        extend:qapp.BetterWindow,

        construct:function ( hub, profileID, profileViewerOnly)
        {
            var that = this;
//            var hub = p_hub;
//            if( hub === undefined) { hub = null; }

            this.base(arguments, hub);

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setWidth( 400);
            this.setHeight( 200);
            this.setContentPadding(0);
            this.setCaption( "...");
            this.setAlwaysOnTop( true);
            // prefix for global variables
            var prefix = "/g1dfs/" + profileID + "/";

            this.m_advancedSettingsButton = this.addToolButton(null,
                    "qapp/icons/swheel12.png").set({
                show: "icon",
                toolTipText: "Open more settings"
            });
            this.m_advancedSettingsButton.addListener("execute", function () {
                this.getProfileWidget().toggleShowOptions();
            }, this);

            this.m_unzoomButton = this.addToolButton("Unzoom");
            this.m_unzoomButton.set({ toolTipText: "Reset zoom to see all data."});
            this.m_unzoomButton.addListener("execute", function () {
                this.m_profileWidget.resetZoom();
            }, this);

            this.m_fitButton = this.addToolButton("Fitter...");
            this.m_fitButton.set({ toolTipText: "Export data to a Gaussian fitter"});
            this.m_fitButton.addListener("execute", function () {
//                this.m_profileWidget.invokeFit();
                this.fireDataEvent("invokeFit");
            }, this);
            if( profileViewerOnly !== true) {
                this.m_fitButton.exclude();
            }

            this.setLayout(new qx.ui.layout.HBox());
            this.m_profileWidget = new qapp.widgets.ProfileWidget( hub, profileID);
            this.add( this.m_profileWidget, { flex: 1});

            // monitor title changes
            fv.makeGlobalVariable( prefix + "title", function( val) {
                that.setCaption( val || "Profile...");
            });

            fv.GLOBAL_DEBUG && fv.console.log( "profile window constructed");
        },

        members:{

            /**
             * Returns the actual profile widget inside this window
             * @returns {*}
             */
            getProfileWidget: function() {
                return this.m_profileWidget;
            }

        },

        properties:{

        }

    });

