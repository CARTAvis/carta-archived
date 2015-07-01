/**
 * Displays a window source with window destinations.
 */

qx.Class.define("skel.widgets.Link.DrawCanvas",{
        extend : qx.ui.embed.Canvas,
        
        /**
         * Constructor.
         */
        construct : function() {
            this.base(arguments);
            this.setSyncDimension(true);

            this.addListener("mousemove", this._mouseMoveCB);
            this.addListener("mousedown", this._mouseDownCB);
            this.addListener("mouseup", this._mouseUpCB);

            // In order to receive key events, activate must be
            // called.
            this.addListener("mouseover", function() {
                this.activate();
            }, this);
            this.addListener("mouseout", function() {
                this.deactivate();
            }, this);

            this.addListener("keypress", this._keyDownCB);
        },

        members : {
            
            /**
             * Adjust the offerings on the context menu based on whether or not the
             * user has clicked close to a line.
             * @param lineMatch {boolean} true if the user has clicked close to a line; false otherwise.
             */
            _adjustContextMenu : function( lineMatch ){
            },

            /**
             * Removes stored link information.
             */
            _clear : function() {
                this.m_sourceLink = null;
                this.m_destLinks = [];
                this.m_linkStart = null;
                this.m_linkEnd = null;
            },

            /**
             * Template method, which can be used by derived classes
             * to redraw the content. It is called each time the
             * canvas dimension change and the canvas needs to be
             * updated.
             * 
             * @param width {Integer} New canvas width
             * @param height {Integer} New canvas height
             * @param ctx {CanvasRenderingContext2D} The
             *                rendering ctx to draw to
             */
            _draw : function(width, height, ctx) {
                if (this.m_sourceLink !== null) {
                    this.base(arguments);
                    
                    ctx.clearRect(0, 0, width, height);
                    
                    ctx.lineWidth = this.m_LINE_WIDTH;
                    ctx.strokeStyle = skel.theme.Color.colors.blank;
                    this._drawPart(ctx);

                    ctx.lineWidth = this.m_LINE_WIDTH_CORE;
                    ctx.strokeStyle = skel.theme.Color.colors.text;
                    this._drawPart(ctx);

                }
            },

            _drawPart : function(ctx) {
                this._drawMark(this.m_sourceLink, ctx);
                for (var i = 0; i < this.m_destLinks.length; i++) {
                    this._drawMark(this.m_destLinks[i], ctx);
                    if (this.m_destLinks[i].linked) {
                        this._drawConnector( this.m_sourceLink.locationX - this.m_left,
                                this.m_sourceLink.locationY - this.m_top,
                                this.m_destLinks[i].locationX - this.m_left,
                                this.m_destLinks[i].locationY - this.m_top,
                                this.m_destLinks[i].twoWay, ctx);
                    }
                }
                this._drawNewLink(ctx);
            },

            _getArrowEnd : function(x0, y0, x1, y1, reverse) {
                var xDiff = x1 - x0;
                var yDiff = y1 - y0;
                var distance = Math.sqrt(Math.pow(xDiff, 2) + Math.pow(yDiff, 2));
                var ANGLE = 30 * Math.PI / 180;
                var cosAngle = Math.cos(ANGLE);
                var sinAngle = Math.sin(ANGLE);
                var ARROW_LENGTH = 50;
                var multiplier = 1;
                if (reverse) {
                    multiplier = -1;
                }
                var scaling = ARROW_LENGTH / distance;
                var xEnd = x1 - (xDiff * cosAngle - yDiff * sinAngle * multiplier) * scaling;
                var yEnd = y1 - (yDiff * cosAngle + xDiff * sinAngle * multiplier) * scaling;
                return {
                    x : xEnd,
                    y : yEnd
                };
            },

            /**
             * Draws a connector between a source point and a
             * destination point.
             * 
             * @param startX {Integer} the x-coordinate of the
             *                source point.
             * @param startY {Integer} the y-coordinate of the
             *                source point.
             * @param endX {Integer} the x-coordinate of the
             *                destination point.
             * @param endY {Integer} the y-coordinate of the
             *                destination point.
             * @param ctx {CanvasRenderingContext2D} the
             *                rendering context.
             */
            _drawConnector : function(startX, startY, endX, endY,
                    biLink, ctx) {
                this._drawLine(startX, startY, endX, endY, ctx);

                var arrow1End = this._getArrowEnd(startX, startY,
                        endX, endY, true);
                this._drawLine(endX, endY, arrow1End.x,
                        arrow1End.y, ctx);

                var arrow2End = this._getArrowEnd(startX, startY,
                        endX, endY, false);
                this._drawLine(endX, endY, arrow2End.x,
                        arrow2End.y, ctx);
                if (biLink) {
                    var arrow3End = this._getArrowEnd(endX, endY,
                            startX, startY, true);
                    this._drawLine(startX, startY, arrow3End.x,
                            arrow3End.y, ctx);

                    var arrow4End = this._getArrowEnd(endX, endY,
                            startX, startY, false);
                    this._drawLine(startX, startY, arrow4End.x,
                            arrow4End.y, ctx);
                }
            },

            _drawLine : function(startX, startY, endX, endY, ctx) {
                ctx.beginPath();

                ctx.moveTo(startX, startY);
                ctx.lineTo(endX, endY);
                ctx.stroke();
            },

            /**
             * Draws a mark indicating a source or destination point
             * for a connector.
             * 
             * @param linkInfo {skel.widgets.LinkInfo} representation
             *                for an end point of a link connector.
             * @param ctx {CanvasRenderingContext2D} the
             *                rendering context.
             */
            _drawMark : function(linkInfo, ctx) {

                // Outer border
                ctx.fillStyle = skel.theme.Color.colors.blank;
                this._drawMarkPart(linkInfo, ctx, this.m_DIAGONAL);

                // Inner mark.
                if (linkInfo.source) {
                    ctx.fillStyle = skel.theme.Color.colors.selection;
                } else {
                    ctx.fillStyle = skel.theme.Color.colors.text;
                }
                this._drawMarkPart(linkInfo, ctx, this.m_DIAGONAL_CORE);
            },

            _drawMarkPart : function(linkInfo, ctx, diagonal) {
                ctx.beginPath();
                var centerX = Math.round(linkInfo.locationX - this.m_left /*- diagonal / 2 */);
                var centerY = Math.round(linkInfo.locationY - this.m_top /*- diagonal / 2 */);
                ctx.arc(centerX, centerY, diagonal, 0, Math.PI * 2);
                ctx.fill();
                ctx.restore();
            },

            /**
             * Draws a link connector from the source link to the
             * current mouse position during an interactive user
             * draw.
             * 
             * @param ctx
             *                {CanvasRenderingContext2D} the
             *                rendering context.
             */

            _drawNewLink : function(ctx) {
                if (this.m_linkStart !== null && this.m_linkEnd !== null) {
                    this._drawLine(this.m_linkStart.x,
                            this.m_linkStart.y, this.m_linkEnd.x,
                            this.m_linkEnd.y, ctx);
                }
            },
            

            /**
             * Returns the link source or destination that matches
             * the passed in point or null if there is no link end
             * point 'close' to the point.
             * 
             * @param pt {Array} consisting of a screen pixel
             *                coordinate.
             */
            _getDestinationMatch : function(pt) {
                var matchingLink = null;
                for (var i = 0; i < this.m_destLinks.length; i++) {
                    if (this._matchesPoint(pt, this.m_destLinks[i])) {
                        matchingLink = this.m_destLinks[i];
                        break;
                    }
                }
                return matchingLink;
            },
            
            /**
             * Return help information for the draw canvas.
             * @return {String} help information for the draw canvas.
             */
            getHelp : function( ){
                return "";
            },
            
            
            /**
             * Event callback when the users types a key on the
             * keyboard.
             * 
             * @param event {qx.event} a keyboard event.
             */
            _keyDownCB : function(event) {
                // Escape key
                if (event.getKeyCode() == 27) {
                    this._quit();
                }
            },
            
            linkCompleted : function( link, pt ){
                
            },

            /**
             * Returns the destination link point of the line
             * connector 'close to' the point or null if there is no
             * line connector near the point.
             * 
             * @param pt {Array} a screen coordinate.
             */
            _matchesLine : function(pt) {
                var matchingDestination = null;
                for (var i = 0; i < this.m_destLinks.length; i++) {
                    if ( this.m_destLinks[i].linked ){
                        var xDiff = this.m_sourceLink.locationX - this.m_destLinks[i].locationX;
                        var distance = this.m_ERROR_MARGIN;
                        if (xDiff !== 0) {
                            var slope = (this.m_sourceLink.locationY - this.m_destLinks[i].locationY) / xDiff;
                            var yIntercept = (this.m_sourceLink.locationY - this.m_top) - slope * (this.m_sourceLink.locationX - this.m_left);
                            var yDist = Math.abs( pt.y - slope * pt.x - yIntercept );
                            distance = yDist / Math.sqrt(slope * slope + 1);
                        } else {
                            distance = Math.abs(pt.x - this.m_sourceLink.locationX);
                        }
    
                        if (distance < this.m_ERROR_MARGIN) {
                            // Make sure the point is in the segment
                            // range.
                            var minX = Math.min(
                                    this.m_sourceLink.locationX,
                                    this.m_destLinks[i].locationX);
                            var maxX = Math.min(
                                    this.m_sourceLink.locationX,
                                    this.m_destLinks[i].locationX);
                            if (minX == maxX || (minX <= pt.x && pt.x <= maxX)) {
                                var minY = Math.min(
                                        this.m_sourceLink.locationY,
                                        this.m_destLinks[i].locationY);
                                var maxY = Math.min(
                                        this.m_sourceLink.locationY,
                                        this.m_destLinks[i].locationY);
                                if (minY == maxY || (minY <= pt.y && pt.y <= maxY)) {
                                    matchingDestination = this.m_destLinks[i];
                                    break;
                                }
                            }
                        }
                    }
                }
                return matchingDestination;
            },

            /**
             * Returns true if the passed in link end point is
             * 'close to' the screen coordinate point; false
             * otherwise.
             * 
             * @param pt {Array} a screen coordinate.
             * @param link {skel.widgets.LinkInfo} a link end point.
             */
            _matchesPoint : function(pt, link) {
                var linkMatch = false;
                var xDistance = Math.abs(pt.x - link.locationX + this.m_left);
                var yDistance = Math.abs(pt.y - link.locationY + this.m_top);
                if (xDistance < this.m_ERROR_MARGIN && yDistance < this.m_ERROR_MARGIN) {
                    linkMatch = true;
                }
                return linkMatch;
            },

            /**
             * Callback for the mouse down event.
             * @param event {qx.event} a mouse event.
             */
            _mouseDownCB : function(ev) {
                if (this.m_touchDevice) return;
                var pt;
                //Start drawing a new connecting link
                if (ev.getButton() === "left") {

                    pt = skel.widgets.Util.localPos(this, ev);
                    var sourceMatch = this._matchesPoint(pt,
                            this.m_sourceLink);
                    if (sourceMatch) {
                        this.capture();
                        this.m_linkStart = {
                            x : this.m_sourceLink.locationX - this.m_left,
                            y : this.m_sourceLink.locationY - this.m_top
                        };
                        this.update();
                    }
                }
                //Show the context menu if the point is "close to" a line connector.
                else if (ev.getButton() == "right") {
                    
                    if (this.m_contextMenu === null) {
                        this.m_contextMenu = new qx.ui.menu.Menu();
                        this.setContextMenu(this.m_contextMenu);
                        var exitButton = new qx.ui.menu.Button( "Exit");
                        exitButton.addListener( "execute", this._quit, this );
                        this.m_contextMenu.add( exitButton );
                    }
                    pt = skel.widgets.Util.localPos(this, ev);
                    var lineMatch = this._matchesLine(pt);
                    this._adjustContextMenu( lineMatch );
                }
            },

            /**
             * Callback for the mouse move event.
             * @param event {qx.event} a mouse event.
             */
            _mouseMoveCB : function(ev) {
                if (this.m_touchDevice)
                    return;
                if (ev.getButton() === "left") {
                    if (this.m_linkStart !== null) {
                        var pt = skel.widgets.Util.localPos(this, ev);
                        this.m_linkEnd = pt;
                        this.update();
                    }
                }
            },

            /**
             * Callback for the mouse up event.
             * @param event {qx.event} a mouse event.
             */
            _mouseUpCB : function(ev) {
                if (this.m_touchDevice)
                    return;
                if (ev.getButton() === "left") {
                    this.releaseCapture();
                    var pt = skel.widgets.Util.localPos(this, ev);
                    //If we are close to a destination link, then add the link.
                    var matchingLink = this
                            ._getDestinationMatch(pt);
                    if (matchingLink !== null) {
                        if (!matchingLink.linked) {
                            var sourceId = this.m_sourceLink.winId;
                            var destId = matchingLink.winId;
                            var link = new skel.widgets.Link.Link( sourceId, destId );
                            matchingLink.linked = true;
                            this.linkCompleted( link, pt );
                        }
                    }
                    //Clear the link endpoints.
                    this.m_linkEnd = null;
                    this.m_linkStart = null;
                    this.update();
                }
            },
            

            /**
             * Stores information about where to draw existing link end points and existing
             * link connectors.
             * @param drawInfo {Array} consisting of skel.widgets.LinkInfo link endpoints.
             */
            setDrawInfo : function(drawInfo) {
                this._clear();
                for (var i = 0; i < drawInfo.length; i++) {
                    if (drawInfo[i].source) {
                        this.m_sourceLink = drawInfo[i];
                    } 
                    else if (drawInfo[i].linkable) {
                        this.m_destLinks.push(drawInfo[i]);
                    }
                }
                this.update();
            },

            /**
             * Stores the offsets for translating between link end points and mouse
             * cursor locations on the screen.
             * @param left {Integer} the pixel correction in the horizontal direction.
             * @param top {Integer} the pixel correction in the vertical direction.
             */
            setLinkOffsets : function(left, top) {
                this.m_left = left;
                this.m_top = top;
                this.update();
            },
            
            /**
             * Exit the canvas (implemented by subclasses).
             */
            _quit : function(){
            },

            m_contextMenu : null,
            m_sourceLink : null,
            m_destLinks : [],
            m_left : 0,
            m_top : 0,
            m_linkEnd : null,
            m_linkStart : null,

            m_LINE_WIDTH : 8,
            m_LINE_WIDTH_CORE : 2,
            m_DIAGONAL : 15,
            m_DIAGONAL_CORE : 10,
            m_ERROR_MARGIN : 25
        }
    });