/**
 * Colormap window for the CyberSKA image viewer.
 * User: pfederl
 */

/* global fv, fv.lib.now */

/**

 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.lib.now)
 @ignore(fv.lib.clamp)
 @ignore(fv.supports.webgl)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(THREE.*)

 @asset(qapp/icons/swheel12.png)

 ************************************************************************ */

qx.Class.define("qapp.widgets.ThreeDsurfacePlotWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base(arguments, hub);

            this.setWidth(390);

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setAlwaysOnTop(true);
            this.setCaption("3D surface plot");
            this.setMinWidth(334);
            this.setMinHeight(200);
            this.setContentPadding(0, 0, 0, 0);
//            this.setIcon( "qapp/icons/histogram16.png");

            // layer1 is where the real UI lives
            this.setLayout(new qx.ui.layout.Grow);
            var layer1 = new qx.ui.container.Composite(new qx.ui.layout.HBox(0));
            this.add( layer1);

            // layer2 is a warning message when there is no data
            var l2layout = new qx.ui.layout.HBox(0);
            l2layout.setAlignX("center");
            l2layout.setAlignY("middle");
            this.m_layer2 = new qx.ui.container.Composite(l2layout);
            this.add( this.m_layer2);
            this.m_layer2.set({backgroundColor: "rgba(0,0,0,0.8)" });
            this.m_info1 = new qx.ui.basic.Label("No data selected...");
            this.m_info1.set({
                allowGrowX: true, allowGrowY: false, backgroundColor: "rgba(255,255,0,0.9)",
                padding: 10
            });
//            this.add( this.m_info1, {left: "0%", right: "0%", top: "0%", bottom: "0%"});
            this.m_layer2.add( this.m_info1);

            // layer3 is a warning message when no webgl is detected
            if( ! fv.supports.webgl) {
                var l3layout = new qx.ui.layout.VBox(0);
                l3layout.setAlignX("center");
                l3layout.setAlignY("middle");
                this.m_layer3 = new qx.ui.container.Composite(l3layout);
                this.add( this.m_layer3);
                this.m_layer3.set({backgroundColor: "rgba(0,0,0,0.8)" });
                var text = "";
                text += "<p style='text-align:center;font-size:1.3em;'>" +
                    "Your browser does not seem to support WebGL.<br />" +
                    "I will try to use lower quality/speed rendering." +
                    "</p>";
                var label = new qx.ui.embed.Html();
                label.set({
                    allowGrowX: true, allowGrowY: true,
                    allowStretchX: true, allowStretchY: true,
                    backgroundColor: "rgba(255,128,128,0.9)",
                    padding: 10
                });
//            this.add( this.m_info1, {left: "0%", right: "0%", top: "0%", bottom: "0%"});
                this.m_layer3.add( label, {flex: 1});
                label.setHtml(text);
                var dismissButton = new qx.ui.form.Button( "Ok");
                dismissButton.set({allowGrowX: false});
                this.m_layer3.add( dismissButton);
                dismissButton.addListener( "execute", function() {
                    this.m_layer3.setVisibility( "excluded");
                }, this);
            }

            this.m_advancedSettingsButton = this.addToolButton(null,
                    "qapp/icons/swheel12.png").set({
                    show: "icon",
                    toolTipText: "Open settings"
                });
            this.m_advancedSettingsButton.addListener("execute", function () {
                if (this.m_moreSettingsGroup.isVisible()) {
                    this.m_moreSettingsGroup.exclude();
                } else {
                    this.m_moreSettingsGroup.show();
                }
            }, this);

            // canvas element
            this.m_canvas = new qx.ui.embed.Canvas().set({
                syncDimension: true
            });
            layer1.add( this.m_canvas, {flex: 1});
            this.m_canvas.addListener("redraw", this._canvasDrawCB, this);
            // TODO: this is a hack to prevent qx.html.embed.Canvas from calling getcontext on its
            // canvas element. A better way would be to create a special 3dcanvas class...
            this.m_canvas.getContentElement().getContext2d = function() {
//                fv.console.log( "getContext2d intercepted, booyah!!!");
            }

            // options
            {
                this.m_moreSettingsGroup = new qx.ui.container.Composite(new qx.ui.layout.VBox(3));
                this.m_moreSettingsGroup.set({ padding: 3})
                //                this.m_moreSettingsGroup.set( "backgroundColor", "#ff0000");
                layer1.add(this.m_moreSettingsGroup, { flex: 0});

                this.m_options = {};
                this.m_options.wireframeToggle = new qx.ui.form.CheckBox( "Wireframe");
                this.m_options.wireframeToggle.addListener( "changeValue",
                    this._uiAction.bind( this, "wireframe"));
                this.m_moreSettingsGroup.add( this.m_options.wireframeToggle);

                this.m_options.autoSpinToggle = new qx.ui.form.CheckBox( "Auto spin");
                this.m_options.autoSpinToggle.set({value:false});
                this.m_options.autoSpinToggle.addListener( "changeValue",
                    this._uiAction.bind( this, "autoSpin"));
                this.m_moreSettingsGroup.add( this.m_options.autoSpinToggle);

                this.m_options.scalingRGroup = new qx.ui.form.RadioGroup();
                this.m_moreSettingsGroup.add( new qx.ui.basic.Label( "Scaling:"));
                var radio = new qx.ui.form.RadioButton( "Histogram");
                this.m_moreSettingsGroup.add( radio);
                this.m_options.scalingRGroup.add( radio);
                radio = new qx.ui.form.RadioButton( "Local");
                this.m_moreSettingsGroup.add( radio);
                this.m_options.scalingRGroup.add( radio);
                this.m_options.scalingRGroup.addListener( "changeSelection", function(e){
                    var selectedButton = e.getData()[0];
                    var label = selectedButton.getLabel();
                    fv.console.log(" text = " + label);
                    this.m_useLocalClips = (label === "Local");
                    this._reconstructGeometry();
                }, this);
                this.m_options.brightnessLabel = new qx.ui.basic.Label( "Brightness: 1.0");
                this.m_moreSettingsGroup.add( this.m_options.brightnessLabel);
                this.m_options.brightnessSlider = new qx.ui.form.Slider();
                this.m_options.brightnessSlider.set({
                    minimum: 10, maximum: 1000, value: 333,
                    singleStep: 10, pageStep: 100,
                    minWidth: 100
                });
                this.m_moreSettingsGroup.add( this.m_options.brightnessSlider);
                this.m_options.brightnessSlider.addListener( "changeValue",
                    this._uiAction.bind( this, "brightness"));
                this.m_useLocalClips = false;
            }


            // if we become active, ask whoever is in charge of the region rectangle
            // to move it to the top
            this.addListener( "changeActive", function() {
                this._emit( "ui.threeDsurfacePlotRect.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);
            this.addListener( "changeVisibility", function() {
                this._emit( "ui.threeDsurfacePlotRect.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);

            // info label


            // register mouse/touch listeners
            // add mouse listeners
            this.m_canvas.addListener("mousedown", this._mouseDownCB, this);
            this.m_canvas.addListener("mousemove", this._mouseMoveCB, this);
            this.m_canvas.addListener("mouseup", this._mouseUpCB, this);
            this.m_canvas.addListener("mousewheel", this._mouseWheelCB, this);
            /*
             this.m_canvas.addListener("touchmove", this._touchMoveCB);
             this.m_canvas.addListener("touchend", this._touchEndCB);
             this.m_canvas.addListener("touchcancel", this._touchCancelCB);
             this.m_canvas.addListener("touchstart", this._touchStartCB);
            */
            this.m_mouseDownPt = null;

            // listen for state changes
            this.m_hub.subscribe("toui.plot3d.setState", this._updateFromState, this);
            this.m_hub.subscribe("toui.plot3d.regionSet", this._regionSetCB, this);
            this.m_hub.subscribe("toui.plot3d.setCachedColormap", this._cachedColormapCB, this);

            // let the controller know we can receive parsed state updates
            this._emit("ui.plot3d.isReady");
        },

        members: {
            m_wglInitialized: false,

            /**
             * returns mouse event's local position (with respect to this widget)
             */
            _localPos: function (event) {
                var box = this.getContentLocation("box");
                return {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
            },

            _mouseDownCB: function (event) {
                if( this.m_touchDevice) return;
                this.m_canvas.capture();
                this.m_mouseDownPt = this._localPos(event);
                this.m_mouseDownAlpha = this.m_viewParams.alpha;
                this.m_mouseDownBeta = this.m_viewParams.beta;
                this.m_mouseDownRadius = this.m_viewParams.radius;
            },

            _mouseUpCB: function (event) {
                this.m_canvas.releaseCapture();
                if( this.m_touchDevice) return;
                this.m_mouseDownPt = null;
            },

            _mouseMoveCB: function (event) {
                if( this.m_mouseDownPt == null) return;
                if( this.m_touchDevice) return;
                // convert event to local widget coordinates
                var pt = this._localPos(event);
                this.m_viewParams.alpha = this.m_mouseDownAlpha + (pt.x - this.m_mouseDownPt.x) * 0.01;
                this.m_viewParams.beta = this.m_mouseDownBeta + (pt.y - this.m_mouseDownPt.y) * 0.01;
                var EPS = 1e-6;
                var MAX_BETA = Math.PI / 2 - EPS;
                var MIN_BETA = 0;
                if( this.m_viewParams.beta > MAX_BETA) this.m_viewParams.beta = MAX_BETA;
                if( this.m_viewParams.beta < MIN_BETA) this.m_viewParams.beta = MIN_BETA;
                this._wglRender();
            },

            _mouseWheelCB: function (event) {
                if( this.m_touchDevice) return;
                var MAX_RADIUS = 200;
                var MIN_RADIUS = 5;
                var r = this.m_viewParams.radius;
                if( event.getWheelDelta() > 0) {
                    r = r * 1.1;
                } else {
                    r = r / 1.1;
                }
                if( r < MIN_RADIUS) r = MIN_RADIUS;
                if( r > MAX_RADIUS) r = MAX_RADIUS;
                this.m_viewParams.radius = r;
                this._wglRender();
            },

            _uiAction: function( cmd /* , val1, val2, val3, ... */ ) {
                if( cmd === "wireframe") {
                    this.m_phongMaterial.wireframe = this.m_options.wireframeToggle.getValue();
                    this.m_phongMaterial.needsUpdate = true;
//                    this.m_phongMaterial.transparent = this.m_phongMaterial.wireframe;
//                    this.m_phongMaterial.opacity = this.m_phongMaterial.wireframe ? 0.5 : 1.0;
                    this._wglRender();
                }
                else if( cmd === "autoSpin") {
                    this._wglAnimate();
                }
                else if( cmd === "brightness") {
                    var val = this.m_options.brightnessSlider.getValue() / 1000;
                    val *= val * 9;
                    fv.console.log( "val = ", val);
                    this.m_light1.intensity = val;
                    this.m_light2.intensity = val;
                    this.m_options.brightnessLabel.setValue( "Brightness: " + val.toFixed(3));
                    this._wglRender();
                }
            },

            _regionSetCB: function (val) {
                // region has been updated
                this._emit( "ui.plot3d.setRegion", val);
            },

            _cachedColormapCB: function (val) {
//                fv.console.log( "cachedColormap:", val);
                this.m_savedCachedColormap = val;
                this._reconstructGeometry();
            },

            _updateFromState: function ( st) {
                this.m_savedState = st;
                fv.console.log( "plot3d._updatefromstate", st);
                if( st.isNull) {
                    this.setCaption("3D surface plot");
                }
                else {
                    this.setCaption("3D surface plot (" + st.nCols + "x" + st.nRows +")");
                }
                this._reconstructGeometry();
            },

            _reconstructGeometry: function ( ) {
                var st = this.m_savedState;
                var cm = this.m_savedCachedColormap;

                if( st == null || st == null || st.isNull ) {
                    this.m_layer2.set({visibility: "visible"});
                    this.m_geometry = new THREE.Geometry();
//                    this.m_geometry.vertices.push( new THREE.Vector3( -10,  10, 0 ) );
//                    this.m_geometry.vertices.push( new THREE.Vector3( -10, -10, 0 ) );
//                    this.m_geometry.vertices.push( new THREE.Vector3(  10, -10, 0 ) );
//                    this.m_geometry.faces.push( new THREE.Face3( 0, 1, 2 ) );
//                    this.m_geometry.computeBoundingSphere();
                }
                else {

                    this.m_layer2.set({visibility: "excluded"});

                    // local closure to be used with ParametericGeometry2...
                    function fun( row, col) {
                        var z, color;
                        var rawVal = st.values[ row * st.nCols + col];
                        if( rawVal !== rawVal) {
                            z = -20;
                            color = 0x000000;
                        }
                        else {
                            var nVal;
                            if( this.m_useLocalClips) {
                                nVal = (rawVal - st.localMin) / (st.localMax - st.localMin);
                                nVal = fv.lib.clamp( nVal, 0, 1);
                            }
                            else {
                                nVal = (rawVal - cm.histMin) / (cm.histMax - cm.histMin);
                                nVal = fv.lib.clamp( nVal, 0, 1);
                            }
                            z = nVal * 20;

                            var ind = Math.round( cm.data.length * (rawVal - cm.histMin) / (cm.histMax - cm.histMin));
                            ind = fv.lib.clamp( ind, 0, cm.data.length-1);
                            color = cm.data[ind];
                        }
                        return {
                            z: z,
                            color: color
                        };
                    }

                    this.m_geometry = new THREE.ParametricGeometry2( fun.bind(this), st.nCols-1, st.nRows-1);
                    this.m_geometry.dynamic = false;

                    var i;

                    this.m_geometry.computeBoundingBox();
                    var zMin = this.m_geometry.boundingBox.min.z;
                    var zMax = this.m_geometry.boundingBox.max.z;
                    var zRange = zMax - zMin;
                    var color, point, face, numberOfSides, vertexIndex;
                    var faceIndices = [ 'a', 'b', 'c', 'd' ];
                    // assign colors to faces from the colors array...
                    for ( i = 0; i < this.m_geometry.faces.length; i++ )
                    {
                        face = this.m_geometry.faces[ i ];
                        numberOfSides = ( face instanceof THREE.Face3 ) ? 3 : 4;
                        for( var j = 0; j < numberOfSides; j++ )
                        {
                            vertexIndex = face[ faceIndices[ j ] ];
                            face.vertexColors[ j ] = this.m_geometry.colors[ vertexIndex ];
                        }
                    }
                }

                // replace old mesh with the new one
                if( this.m_mesh != null) {
                    this.m_scene.remove( this.m_mesh );
                }
                this.m_mesh = new THREE.Mesh( this.m_geometry, this.m_phongMaterial);
                this.m_scene.add( this.m_mesh );

                // render current scene
                this._wglRender();
            },

            // callback to redraw the overlay
            _canvasDrawCB: function (e) {
                // call _wglInit once
                if( ! this.m_wglInitialized) {
                    this._wglInit();
                    this.m_wglInitialized = true;
                }
                this._wglOnResize();
            },

            _wglInit: function() {
                this.m_camera = new THREE.PerspectiveCamera(
                    60, this.m_canvas.getCanvasWidth() / this.m_canvas.getCanvasHeight(), 1, 300 );
                this.m_camera.position.set(6000,6000,1300);
                this.m_camera.up = new THREE.Vector3(0,0,1);
                this.m_camera.lookAt(new THREE.Vector3(0,0,0));

                this.m_scene = new THREE.Scene();
//                this.m_scene.fog = new THREE.FogExp2( 0xaaccff, 0.00007 );

                this.m_phongMaterial = new THREE.MeshPhongMaterial({
                    ambient: 0x333333,
                    color: 0xcccccc,
                    specular: 0xffffff,
                    shininess: 2.5,
                    side: THREE.DoubleSide,
                    vertexColors: THREE.VertexColors,
                    shading: THREE.FlatShading,
                    wireframe: false,
                    metal: false
                });

                // FLOOR
//                var floorTexture = new THREE.ImageUtils.loadTexture( fv.images.checkerboard );
//                floorTexture.wrapS = floorTexture.wrapT = THREE.RepeatWrapping;
//                floorTexture.repeat.set( 10, 10 );
//                var floorMaterial = new THREE.MeshBasicMaterial( { map: floorTexture, side: THREE.DoubleSide } );
//                var floorGeometry = new THREE.PlaneGeometry(1000, 1000, 10, 10);
//                var floor = new THREE.Mesh(floorGeometry, floorMaterial);
//                this.m_scene.add(floor);

/*
                var skyBoxGeometry = new THREE.CubeGeometry( 100, 100, 40 );
                var skyBoxMaterial = new THREE.MeshBasicMaterial( {
                    color: 0x9999ff, side: THREE.BackSide,
                    wireframe: true
                } );
                var skyBox = new THREE.Mesh( skyBoxGeometry, skyBoxMaterial );
                this.m_scene.add(skyBox);
*/

                var s = 52;
                var box = new THREE.Geometry();
                box.vertices.push( new THREE.Vector3( -s,-s,0 ));
                box.vertices.push( new THREE.Vector3(  s,-s,0 ));
                box.vertices.push( new THREE.Vector3(  s, s,0 ));
                box.vertices.push( new THREE.Vector3( -s, s,0 ));
                box.vertices.push( new THREE.Vector3( -s,-s,0 ));
//                box.colors.push( new THREE.Color( 0xffffff ));
//                box.colors.push( new THREE.Color( 0xffffff ));
//                box.colors.push( new THREE.Color( 0xffffff ));
//                box.colors.push( new THREE.Color( 0xffffff ));
//                box.colors.push( new THREE.Color( 0xffffff ));
//                var skyBoxMaterial = new THREE.MeshBasicMaterial( {
//                    color: 0x9999ff, side: THREE.BackSide,
//                    wireframe: true
//                } );
//                var line = new THREE.Line( skyBoxGeometry, new THREE.LineBasicMaterial( {
//                    color: 0xffffff, opacity: 1, linewidth: 3, vertexColors: THREE.VertexColors } ) );
//                var lineMat = new THREE.LineDashedMaterial( {
//                    color: 0xaaaaaa, linewidth: 1, //  vertexColors: THREE.VertexColors,
//                    dashSize: 1, gapSize: 10
//                } );
                box.computeLineDistances();
                var lineMat = new THREE.LineDashedMaterial( { color: 0xaaaaaa, dashSize: 3, gapSize: 1, linewidth: 2 } );
                var line = new THREE.Line( box, lineMat );

/*
                var skyBoxMaterial = new THREE.LineBasicMaterial(  );
                var skyBox = new THREE.Mesh( skyBoxGeometry, skyBoxMaterial );
                this.m_scene.add(skyBox);
*/
                this.m_scene.add( line);

                /*
                                var worldWidth = 50;
                                var worldDepth = 50;
                                this.m_geometry = new THREE.PlaneGeometry( 100, 100, worldWidth - 1, worldDepth - 1 );
                                this.m_geometry.dynamic = false;

                                var i, il;

                                for ( i = 0, il = this.m_geometry.vertices.length; i < il; i ++ ) {
                                    var x = this.m_geometry.vertices[ i ].x;
                                    var y = this.m_geometry.vertices[ i ].y;
                                    var r = x*x + y*y;
                                    r = Math.sqrt( r);
                                    r /= Math.sqrt( 50*50*2);
                                    r = Math.cos( r * Math.PI *2);
                                    this.m_geometry.vertices[ i ].z = r * 20 + Math.random() * 5;
                                }

                                this.m_geometry.computeFaceNormals();
                                this.m_geometry.computeVertexNormals();

                                this.m_geometry.computeBoundingBox();
                                var zMin = this.m_geometry.boundingBox.min.z;
                                var zMax = this.m_geometry.boundingBox.max.z;
                                var zRange = zMax - zMin;
                                var color, point, face, numberOfSides, vertexIndex;
                                var faceIndices = [ 'a', 'b', 'c', 'd' ];
                                for ( i = 0; i < this.m_geometry.vertices.length; i++ )
                                {
                                    point = this.m_geometry.vertices[ i ];
                                    color = new THREE.Color( 0x0000ff );
                                    color.setHSL( 0.7 * (zMax - point.z) / zRange, 1, 0.5 );
                                    this.m_geometry.colors[i] = color;
                                }
                                for ( i = 0; i < this.m_geometry.faces.length; i++ )
                                {
                                    face = this.m_geometry.faces[ i ];
                                    numberOfSides = ( face instanceof THREE.Face3 ) ? 3 : 4;
                                    for( var j = 0; j < numberOfSides; j++ )
                                    {
                                        vertexIndex = face[ faceIndices[ j ] ];
                                        face.vertexColors[ j ] = this.m_geometry.colors[ vertexIndex ];
                                    }
                                }

                                this.m_mesh = new THREE.Mesh( this.m_geometry, this.m_phongMaterial);
                                this.m_scene.add( this.m_mesh );
                */
                this.m_mesh = null;

                this.m_light1 = new THREE.PointLight( 0xffffff, 1.0 );
                this.m_light1.position.set( 1000, -1000, 2000 );
                this.m_scene.add( this.m_light1 );

                this.m_light2 = new THREE.DirectionalLight( 0xffffff, 1.0 );
                this.m_light2.position.set( -1000, 1000, 2 );
                this.m_scene.add( this.m_light2 );

//                var object = new THREE.AxisHelper( 100 );
//                object.position.set( -51,-51,0 );
//                this.m_scene.add( object );

/*
                var object = new THREE.ArrowHelper( new THREE.Vector3( 1, 0, 0 ), new THREE.Vector3(0,0, 0 ), 110, 0xff0000 );
                object.position.set( -51,-51,0 );
                this.m_scene.add( object );
                object = new THREE.ArrowHelper( new THREE.Vector3( 0, 1, 0 ), new THREE.Vector3(0,0, 0 ), 110, 0x00ff00 );
                object.position.set( -51,-51,0 );
                this.m_scene.add( object );
*/

                var params = {
                    alpha: true,
                    canvas: this.m_canvas.getContentElement().getDomElement(),
                    antialias: true
                };
                if (fv.supports.webgl) {
                    this.m_renderer = new THREE.WebGLRenderer( params);
                } else {
                    this.m_renderer = new THREE.CanvasRenderer( params);
                }
//                this.m_renderer.setClearColor( 0xaaccff, 1 );
                this.m_renderer.setClearColor( 0x000000, 1 );
                this.m_renderer.setSize( this.m_canvas.getCanvasWidth(), this.m_canvas.getCanvasHeight() );

//                stats = new Stats();
//                stats.domElement.style.position = 'absolute';
//                stats.domElement.style.top = '0px';
//                container.appendChild( stats.domElement );

//                window.addEventListener( 'resize', onWindowResize, false );

                this.m_viewParams = {
                    alpha: Math.PI,
                    beta: Math.PI / 4,
                    radius: 100
                };

                this._wglAnimate();
            },

            _wglOnResize: function() {
                var width = this.m_canvas.getCanvasWidth();
                var height = this.m_canvas.getCanvasHeight();
                fv.console.log( "wglonresize " + width + "x" + height);
                this.m_camera.aspect = width / height;
                this.m_camera.updateProjectionMatrix();

                if( this.m_renderer.setViewport) {
                    this.m_renderer.setViewport( 0, 0, width, height);
                } else {
                    this.m_renderer.setSize( this.m_canvas.getCanvasWidth(), this.m_canvas.getCanvasHeight() );
                }
                this._wglRender();
            },

            _wglAnimate: function() {

                if( ! this.m_options.autoSpinToggle.getValue()) {
                    this._wglRender();
                    return;
                }
                var time = Date.now() * 0.001;

                this.m_viewParams.alpha += Math.PI / 60 / 10;

                this._wglRender();

                qx.bom.AnimationFrame.request( this._wglAnimate.bind( this));
//                window.requestAnimationFrame( this._wglAnimate.bind( this));
            },

            _wglRender: function() {
                var vp = this.m_viewParams;
                var cx = Math.sin( vp.alpha) * vp.radius;
                var cy = Math.cos( vp.alpha) * vp.radius;
                var cz = Math.sin( vp.beta) * vp.radius;
                cx *= Math.cos( vp.beta);
                cy *= Math.cos( vp.beta);

                this.m_camera.position.set( cx, cy, cz);
                this.m_camera.up = new THREE.Vector3(0,0,1);
                this.m_camera.lookAt(new THREE.Vector3(0,0,0));
                this.m_renderer.render( this.m_scene, this.m_camera );
            }
        },

        properties: {

        }

    });

