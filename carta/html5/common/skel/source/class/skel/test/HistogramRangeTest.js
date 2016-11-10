qx.Class.define("skel.test.HistogramRangeTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_rangeWidget = new skel.widgets.Histogram.HistogramRange();
        },

        tearDown : function() {
            this.m_rangeWidget.dispose();
            this.m_rangeWidget = null;
        },
        


        /**
         * Test that if we enable/disable the buffer the corresponding
         * text field changes its enabled status.
         */
        testBufferEnable : function() {
            this.m_rangeWidget.setBuffer( true );
            this.assertTrue( this.m_rangeWidget.m_bufferText.isEnabled() );
            this.m_rangeWidget.setBuffer( false );
            this.assertFalse( this.m_rangeWidget.m_bufferText.isEnabled() );
        },
        
        /**
         * Test that we cannot set the maximum buffer amount to a value larger
         * than 110.
         */
        testBufferMax : function(){
        	this.m_rangeWidget.setBuffer( true );
        	this.m_rangeWidget.setBufferAmount( 110 );
        	this.assertTrue( this.m_rangeWidget.m_bufferText._isWarning() );
        	this.m_rangeWidget.setBufferAmount( 5 );
        	this.assertFalse( this.m_rangeWidget.m_bufferText._isWarning() );
        },
        
        /**
         * Test that we cannot set the minimum buffer amount to a value less
         * than 0.
         */
        testBufferMin : function(){
        	this.m_rangeWidget.setBuffer( true );
        	this.m_rangeWidget.setBufferAmount( -10 );
        	this.assertTrue( this.m_rangeWidget.m_bufferText._isWarning() );
        	this.m_rangeWidget.setBufferAmount( 5 );
        	this.assertFalse( this.m_rangeWidget.m_bufferText._isWarning() );
        },
        
        /**
         * Test that we cannot set the max percent to a value larger than 100.
         */
        testPercentMax : function(){
        	this.m_rangeWidget.setClipPercents( 0.5, 150 );
        	this.assertTrue( this.m_rangeWidget.m_percentMaxClipText._isWarning());
        	this.m_rangeWidget.setClipPercents( 0.5, 0.9 );
        	this.assertFalse( this.m_rangeWidget.m_percentMaxClipText._isWarning());
        },
        
        /**
         * Test that we cannot set the min percent to a value less than 0.
         */
        testPercentMin : function(){
        	this.m_rangeWidget.setClipPercents( -0.5, 50 );
        	this.assertTrue( this.m_rangeWidget.m_percentMinClipText._isWarning());
        	this.m_rangeWidget.setClipPercents( 0.5, 90 );
        	this.assertFalse( this.m_rangeWidget.m_percentMinClipText._isWarning());
        },
        
        m_rangeWidget : null

    }
});
