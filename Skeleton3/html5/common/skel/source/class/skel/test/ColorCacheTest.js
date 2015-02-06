qx.Class.define("skel.test.ColorCacheTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_cacheWidget = new skel.widgets.Colormap.ColorCache();
        },

        tearDown : function() {
            this.m_cacheWidget.dispose();
            this.m_cacheWidget = null;
        },
        


        /**
         * Test that if caching is set to false, interpolation and cache size are disabled.
         */
        testNoCaching : function() {
            this.m_cacheWidget.setCache( false );
            this.assertFalse( this.m_cacheWidget.m_interpolateCheck.isEnabled());
            this.assertFalse( this.m_cacheWidget.m_cacheSizeText.isEnabled());
        },
        
        /**
         * Test that if caching is set to true, interpolation and cache size are enabled.
         */
        testCaching : function(){
        	this.m_cacheWidget.setCache( true );
        	this.assertTrue( this.m_cacheWidget.m_interpolateCheck.isEnabled());
            this.assertTrue( this.m_cacheWidget.m_cacheSizeText.isEnabled());
        },

        

        m_cacheWidget : null

    }
});
