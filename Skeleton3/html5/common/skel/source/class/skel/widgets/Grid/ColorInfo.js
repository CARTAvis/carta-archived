/**
 * Encapsulation of a color.
 */

qx.Class.define("skel.widgets.Grid.ColorInfo", {
    extend : qx.core.Object,

    /**
     * Constructor.
     */
    construct : function() {
    },
    
    members : {
        
        /**
         * Return the amount of red.
         * @return {Number} - the amount of red.
         */
        getRed : function(){
            return this.m_red;
        },
        
        /**
         * Returns the amount of green.
         * @return {Number} - the amount of green.
         */
        getGreen : function(){
            return this.m_green;
        },
        
        /**
         * Returns the amount of blue.
         * @return {Number} -the amount of blue.
         */
        getBlue : function(){
            return this.m_blue;
        },
        
        /**
         * Sets the amount of red.
         * @param amount {Number} the amount of red.
         */
        setRed : function( amount ){
            this.m_red = amount;
        },
        
        /**
         * Sets the amount of green.
         * @param amount {Number} the amount of green.
         */
        setGreen : function( amount ){
            this.m_green = amount;
        },
        
        /**
         * Sets the amount of blue.
         * @param amount {Number}-the amount of blue.
         */
        setBlue : function( amount ){
            this.m_blue = amount;
        },
        
        m_red : 255,
        m_green : 255,
        m_blue : 255

    }
});