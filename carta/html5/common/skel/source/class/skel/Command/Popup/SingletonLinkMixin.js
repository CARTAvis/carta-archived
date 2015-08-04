
/**
 * Determines if there is an existing link to a plugin of a given type.
 */

qx.Mixin.define("skel.Command.Popup.SingletonLinkMixin", {

        members : {
            
            /**
             * Returns the contents of the user preferences shared variable.
             * @param windows {Array} - list of all in-line windows.
             * @return {boolean} - true if there is a linked in-line window already
             *          displaying the plugin; false otherwise.
             */
            isLinked : function( windows ){
                var activeWins = skel.Command.Command.m_activeWins;
                var linked = false;
                if ( activeWins !== null && activeWins.length > 0 ){
                    var activeWinCount = activeWins.length;
                    var path = skel.widgets.Path.getInstance();
                    for ( var i = 0; i < activeWinCount; i++ ){
                        if ( activeWins[i].getPlugin() !== this.m_plugin ){
                            //Now go through the list of all the windows and see if
                            //there is one with a matching plugin that is linked to this
                            //active window.
                            var winCount = windows.length;
                            for ( var j = 0; j < winCount; j++ ){
                                if ( windows[j].getPlugin() == this.m_plugin ){
                                    var links = windows[j].getLinks();
                                    var linkCount = links.length;
                                    //See if the id of this window is one of our links.
                                    for ( var k = 0; k < linkCount; k++ ){
                                        if ( links[k] == activeWins[i].getIdentifier() ){
                                            linked = true;
                                            break;
                                        }
                                    }
                                    if ( linked ){
                                        break;
                                    }
                                }
                            }
                        }
                        else {
                            linked = true;
                            break;
                        }
                        if ( linked ){
                            break;
                        }
                    }

                }
                return linked;
            },
            
            m_plugin : null
        }
});
