qx.Class.define("skel.test.ContoursTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_contourPage = new skel.widgets.Image.Contour.GeneratorPage();
        },

        tearDown : function() {
            this.m_contourPage.dispose();
            this.m_contourPage = null;
        },
        
        /**
         * Test that the level count cannot be set less than 1.
         */
        testLevelCountMin : function(){
        	this.m_contourPage._setLevelCount( 0 );
        	var levelCount = this.m_contourPage.m_levelCountSpin.getValue();
        	this.assertTrue( levelCount > 0 );
        	this.m_contourPage._setLevelCount( 5 );
        	levelCount = this.m_contourPage.m_levelCountSpin.getValue();
        	this.assertTrue( levelCount == 5 );
        },

        
        /**
         * Test that we cannot set the interval to a negative number
         */
        testIntervalNegative : function(){
        	this.m_contourPage._setGenerateMethod( "Minimum" );
        	this.m_contourPage._setInterval( -10 );
        	this.assertTrue( this.m_contourPage.m_intervalWidget._isWarning() );
        	this.m_contourPage._setInterval(0.5 );
        	this.assertFalse( this.m_contourPage.m_intervalWidget._isWarning() );
        },
        
        m_contourPage : null
    }
});
