/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.hub.emit)
 @ignore(fv.GLOBAL_DEBUG)

 @asset(qapp/icons/movie-play16.png)
 @asset(qapp/icons/movie-stop16.png)
 @asset(qapp/icons/movie-play-reverse16.png)
 @asset(qapp/icons/histogram16.png)
 @asset(qapp/icons/swheel12.png)
 @asset(qapp/icons/movie16.png)
 @asset(qapp/icons/movie-next-frame16.png)
 @asset(qapp/icons/movie-previous-frame16.png)
 @asset(qapp/icons/rock-and-roll16.png)

 ************************************************************************ */

/**
 Interface description
 =====================

 Listens for incoming messages on the hub:

 toui.moviePlayer.setState val

 where val has this format:

 {
     currentFrame: integer
     numFrames: integer
     frameDelay: integer (in milliseconds)
     status: "forward" | "backwards" | "stopped"
     isBouncing: boolean
     startFrame: integer
     endFrame: integer
     frameSkip: integer
 }

 Emits the following messages to the hub

 ui.moviePlayer.isReady with no parameters
 - this is sent when the movie player widget is operational, and indicates to the controller
 that it's ready to receive messages
 ui.moviePlayer.request with command and parameters
 - this is sent as a result of user interaction

 commands and parameters have this format
 {
     cmd: <string>
     arg0: <any type> [optional]
     ...
     argn: <any type> [optional]
 }

 Commands:
 play
 rplay
 stop
 setFrame val <integer>
 setFrameDelay val <integer>
 setBounce <boolean>
 Examples:
 emit( "moviePlayer.request", { cmd: "setFrame", val: 11 });
 emit( "moviePlayer.request", { cmd: "play" });
 */

qx.Class.define("qapp.widgets.MoviePlayerWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base(arguments, hub);

//            this.m_hub = hub;

            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setAlwaysOnTop(true);
//            this.setOpacity(0.9);
            this.setWidth(200);
//            this.setHeight( 500);
            this.setLayout(new qx.ui.layout.VBox(3));
            this.setCaption("Movie controls");
            this.setContentPadding(3);
            this.setIcon("qapp/icons/movie16.png");

            this.m_inUpdateState = false;

/*
            this.m_advancedSettingsButton = this.addToolButton(null, "qapp/icons/swheel12.png").set({
                show: "icon",
                toolTipText: "Open more settings"
            });
            this.m_advancedSettingsButton.addListener("execute", function () {
                if (this.m_moreSettingsGroup.isVisible()) {
                    this.m_moreSettingsGroup.exclude();
                } else {
                    this.m_moreSettingsGroup.show();
                }
            }, this);
*/


            {
                var con3 = new qx.ui.container.Composite(new qx.ui.layout.HBox);
                this.add(con3);
                con3.add(new qx.ui.basic.Label("Current frame:"));
                this.m_currentFrameLabel = new qx.ui.basic.Label("n/a");
                con3.add(this.m_currentFrameLabel);
                con3.add(new qx.ui.core.Spacer, { flex: 3 });
                con3.add(new qx.ui.basic.Label("FPS:"));
                this.m_fpsLabel = new qx.ui.basic.Label("n/a");
                con3.add(this.m_fpsLabel);
            }

            {
                this.m_frameSlider = new qapp.widgets.GrSlider();
                this.add(this.m_frameSlider);
                this.m_frameSlider.addListener("changeValue", function () {
                    var f = this.m_frameSlider.getValue();
                    fv.GLOBAL_DEBUG && fv.console.log("requesting frame ", f);
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrame", val: f });
                }, this);
            }

            {
                var con1 = new qx.ui.container.Composite(new qx.ui.layout.HBox(5));
                this.add(con1);
                con1.add(new qx.ui.core.Spacer, { flex: 3 });
                this.m_previousFrameButton = new qx.ui.form.Button(
                    "Next frame", "qapp/icons/movie-previous-frame16.png")
                    .set({ show: "icon", iconPosition: "top", focusable: false,
                        toolTipText: "Go to previous frame"});
                this.m_previousFrameButton.addListener("execute", function () {
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrame", val: this.m_frameSlider.getValue() - 1 });
                }, this);
                con1.add(this.m_previousFrameButton);
                con1.add(new qx.ui.core.Spacer, { flex: 3 });
                this.m_reverseButton = new qx.ui.form.ToggleButton(
                    "Reverse", "qapp/icons/movie-play-reverse16.png")
                    .set({ show: "icon", iconPosition: "top", focusable: false,
                        toolTipText: "Play movie in reverse"});
                this.m_reverseButton.addListener("execute", function () {
                    this._emit("ui.moviePlayer.request",
                        { cmd: "rplay" });
                }, this);
                con1.add(this.m_reverseButton);
                con1.add(new qx.ui.core.Spacer, { flex: 1 });
                this.m_stopButton = new qx.ui.form.ToggleButton(
                    "Stop", "qapp/icons/movie-stop16.png")
                    .set({ show: "icon", iconPosition: "top", focusable: false,
                        toolTipText: "Stop playing movie"});
                this.m_stopButton.addListener("execute", function () {
                    this._emit("ui.moviePlayer.request",
                        { cmd: "stop" });
                }, this);
                con1.add(this.m_stopButton);
                con1.add(new qx.ui.core.Spacer, { flex: 1});
                this.m_forwardButton = new qx.ui.form.ToggleButton(
                    "Play", "qapp/icons/movie-play16.png")
                    .set({ show: "icon", iconPosition: "top", focusable: false,
                        toolTipText: "Play movie"});
                this.m_forwardButton.addListener("execute", function () {
                    this._emit("ui.moviePlayer.request",
                        { cmd: "play" });
                }, this);
                con1.add(this.m_forwardButton);
                con1.add(new qx.ui.core.Spacer, { flex: 3 });
                this.m_nextFrameButton = new qx.ui.form.Button(
                    "Next frame", "qapp/icons/movie-next-frame16.png")
                    .set({ show: "icon", iconPosition: "top", focusable: false,
                        toolTipText: "Go to next frame"});
                this.m_nextFrameButton.addListener("execute", function () {
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrame", val: this.m_frameSlider.getValue() + 1 });
                }, this);
                con1.add(this.m_nextFrameButton);
                con1.add(new qx.ui.core.Spacer, { flex: 3 });
                this.m_bouncingToggle = new qx.ui.form.ToggleButton("Rock & Roll",
                    "qapp/icons/rock-and-roll16.png")
                    .set({show: "icon",
                        toolTipText: "If toggled, movie will play back and forth.",
                        focusable: false});
                this.m_bouncingToggle.addListener( "changeValue", function(){
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setBouncing", val: this.m_bouncingToggle.getValue()});

                }, this);
                con1.add(this.m_bouncingToggle);
                con1.add(new qx.ui.core.Spacer, { flex: 3 });
            }

            {
                var con2 = new qx.ui.container.Composite( new qx.ui.layout.HBox);
                this.add(con2);
//                var expandSettingsButton = new qx.ui.form.Button("More settings...")
                var expandSettingsButton = new qapp.boundWidgets.CheckBox("More settings...", "/MoviePlayer/moreSettings")
                    .set({
                        padding: [0,2],
                        toolTipText: "Show/hide more settings",
                        focusable: false
                    });
                con2.add( expandSettingsButton);
                expandSettingsButton.addListener("changeValue", function (val) {
                    if (! val.getData()) {
                        this.m_moreSettingsGroup.exclude();
                    } else {
                        this.m_moreSettingsGroup.show();
                    }
                }, this);
            }

            {
//                this.m_moreSettingsGroup = new qx.ui.groupbox.GroupBox("More settings").set({
//                    contentPadding: 3,
//                    layout: new qx.ui.layout.Grow(),
//                    visibility: "excluded"
//                });
//                this.m_moreSettingsGroup = new qx.ui.container.Composite().set({
//                    padding: 3,
//                    layout: new qx.ui.layout.Grow(),
//                    visibility: "excluded"
//                });
//                this.add(this.m_moreSettingsGroup);

                this.m_moreSettingsGroup = new qx.ui.container.Composite(new qx.ui.layout.Grid(4, 4))
                    .set({
                        visibility: "excluded",
                        allowGrowX: true
                    });
                var msi = this.m_moreSettingsGroup; // alias
                this.add(msi);

                var row = 0;
                msi.add(new qx.ui.basic.Label("Current frame:"), { row: row, column: 0});
                this.m_currentFrameSpinner = new qx.ui.form.Spinner();
                this.m_currentFrameSpinner.addListener("changeValue", function () {
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrame", val: this.m_currentFrameSpinner.getValue() });
                }, this);
                msi.add(this.m_currentFrameSpinner, { row: row, column: 1});
//                row++;
                msi.add(new qx.ui.basic.Label("Frame delay:"), { row: row, column: 2});
                this.m_frameDelaySpinner = new qx.ui.form.Spinner();
                this.m_frameDelaySpinner.addListener("changeValue", function () {
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrameDelay", val: this.m_frameDelaySpinner.getValue()});
                }, this);
                msi.add(this.m_frameDelaySpinner, { row: row, column: 3});
                row++;
                msi.add(new qx.ui.basic.Label("Start frame:"), { row: row, column: 0});
                this.m_startFrameSpinner = new qx.ui.form.Spinner();
                this.m_startFrameSpinner.addListener("changeValue", function () {
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setStartFrame", val: this.m_startFrameSpinner.getValue() });
                }, this);
                msi.add(this.m_startFrameSpinner, { row: row, column: 1});
//                row++;
                msi.add(new qx.ui.basic.Label("End frame:"), { row: row, column: 2});
                this.m_endFrameSpinner = new qx.ui.form.Spinner();
                this.m_endFrameSpinner.addListener("changeValue", function () {
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setEndFrame", val: this.m_endFrameSpinner.getValue() });
                }, this);
                msi.add(this.m_endFrameSpinner, { row: row, column: 3});
                row++;
                msi.add(new qx.ui.basic.Label("Skip factor:"), { row: row, column: 0});
                this.m_frameSkipSpinner = new qx.ui.form.Spinner();
                this.m_frameSkipSpinner.addListener("changeValue", function () {
                    if (this.m_inUpdateState) return;
                    this._emit("ui.moviePlayer.request",
                        { cmd: "setFrameSkip", val: this.m_frameSkipSpinner.getValue()});
                }, this);
                msi.add(this.m_frameSkipSpinner, { row: row, column: 1});
                row++;
                row++;
                var kvisToggle = new qx.ui.form.CheckBox("Single button mouse control")
                    .set({ value: false});
                kvisToggle.addListener("changeValue", function () {
                    this.m_frameSlider.setKvisStyle(!kvisToggle.getValue());
                }, this);
                this.m_frameSlider.setKvisStyle(!kvisToggle.getValue());
                msi.add(kvisToggle,
                    { row: row, column: 1, colSpan: 4});

                msi.getLayout().setColumnAlign(0, "right", "middle");
                msi.getLayout().setColumnAlign(1, "left", "middle");
                msi.getLayout().setColumnAlign(2, "right", "middle");
                msi.getLayout().setColumnAlign(3, "left", "middle");
                msi.getLayout().setColumnFlex(0, 1);
                msi.getLayout().setColumnFlex(1, 1);
                msi.getLayout().setColumnFlex(2, 1);
                msi.getLayout().setColumnFlex(3, 1);
                msi.getLayout().setRowAlign(3, "left", "bottom");

            }


            this.m_hub.subscribe("toui.moviePlayer.setState", function (state) {
                this._updateState(state);
            }, this);

            // let the controller know we can receive parsed state updates
            this._emit("ui.moviePlayer.isReady");
//            this._emit("ui.parsedStateRequest");

            fv.console.log("Player window constructed.");
        },

        members: {

            m_lastFrameTime: 0,
            m_lastFrameStamped: -1,
            m_fps: 0,

            _updateState: function (state) {
                this.m_inUpdateState = true;
//                fv.GLOBAL_DEBUG && fv.console.group("movie player is receiving state update %O", state);
                this.setOpacity( state.numFrames > 1 ? 1 : 0.5);

                this.m_currentFrameLabel.setValue((state.currentFrame) + "/" + state.numFrames);
                this.m_frameSlider.setMinimum(state.startFrame);
                this.m_frameSlider.setMaximum(state.endFrame);
                this.m_frameSlider.setValue(state.currentFrame);
                this.m_currentFrameSpinner.set({
                    minimum: 1,
                    maximum: state.numFrames,
                    value: state.currentFrame
                });
                this.m_frameDelaySpinner.set({
                    minimum: 0,
                    maximum: 99999,
                    value: state.frameDelay
                });
                this.m_startFrameSpinner.set({
                    minimum: 1,
                    maximum: state.numFrames,
                    value: state.startFrame
                });
                this.m_endFrameSpinner.set({
                    minimum: 1,
                    maximum: state.numFrames,
                    value: state.endFrame
                });
                this.m_frameSkipSpinner.set({
                    minimum: 1,
                    maximum: state.numFrames,
                    value: state.frameSkip
                });

                this.m_reverseButton.setValue(state.status === "rplay");
                this.m_forwardButton.setValue(state.status === "play");
                this.m_stopButton.setValue(state.status === "stop");

                this.m_bouncingToggle.setValue( state.isBouncing);

                // if this frame is different from the previous frame, record how long it took
                // and update fps based on this
                if( state.currentFrame !== this.m_lastFrameStamped) {
                    this.m_lastFrameStamped = state.currentFrame;
                    var now = Date.now();
                    var diff = now - this.m_lastFrameTime;
                    this.m_lastFrameTime = now;
                    if( diff < 1) diff = 1;
                    var fps = 1000 / diff;
                    var ma = 0.1;
                    this.m_fps = ma * fps + (1-ma) * this.m_fps;
                    this.m_fpsLabel.setValue( this.m_fps.toPrecision(3));
                }
                this.m_inUpdateState = false;

            }


        },

        properties: {

        }

    });

