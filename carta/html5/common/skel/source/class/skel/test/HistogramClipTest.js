qx.Class.define("skel.test.HistogramClipTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_clipWidget = new skel.widgets.Histogram.HistogramClip();
        },

        tearDown : function() {
            this.m_clipWidget.dispose();
            this.m_clipWidget = null;
        },
        
        /**
         * Test that we cannot set the max clip percent to a value larger than 100.
         */
        testPercentMax : function(){
        	this.m_clipWidget.setColorRangePercent( 0.5, 150 );
        	this.assertTrue( this.m_clipWidget.m_percentMaxClipText._isWarning());
        	this.m_clipWidget.setColorRangePercent( 0.5, 0.9 );
        	this.assertFalse( this.m_clipWidget.m_percentMaxClipText._isWarning());
        },
        
        /**
         * Test that we cannot set the min clip percent to a value less than 0.
         */
        testPercentMin : function(){
        	this.m_clipWidget.setColorRangePercent( -0.5, 50 );
        	this.assertTrue( this.m_clipWidget.m_percentMinClipText._isWarning());
        	this.m_clipWidget.setColorRangePercent( 0.5, 90 );
        	this.assertFalse( this.m_clipWidget.m_percentMinClipText._isWarning());
        },
        
        m_clipWidget : null

    }
});
