/**
 * Converts image/mouse coordinates.
 */

qx.Class.define("skel.widgets.Draw.ImageMouseTranslator", {
    extend : qx.core.Object,

    construct : function( winId){
        //TODO Initialize a command to get a shared variable to listen to 
        //transformation changes.
    },
    members : {
        serverImage2mouse: function (pt) {
            if (this.m_tx === null) return pt;
            return this.m_tx.serverImage2mousef(pt);
        },
        
        _setTxCB: function (val) {
//          fv.GLOBAL_DEBUG && fv.console.log("tx =", val);
          this.m_tx = val;
        },
        
        mouse2serverImage: function (pt) {
            if (this.m_tx === null) return pt;
            return this.m_tx.mouse2serverImagef(pt);
        },
        
        m_tx : null
    }
});