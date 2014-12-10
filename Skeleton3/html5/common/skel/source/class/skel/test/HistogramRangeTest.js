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
        
        isWarningPosted : function( textField ){
            var warningPosted = false;
            var warningIndex = textField.indexOf( textField.m_warning );
            if ( warningIndex >= 0 ){
                warningPosted = true;
            }
            return warningPosted;
        },

        /**
         * Test that if a minimum value larger than a maximum value is set a warning is posted.
         * The warning should then disappear if a smaller minimum value is set.
         */
        testTooLargeMinValue : function() {
            this.m_rangeWidget.m_maxClipText.setValue(50);
            this.assertEquals( this.m_rangeWidget.m_maxClipText.getValue(), 50 );
            this.m_rangeWidget.m_minClipText.setValue( 75 );
            this.assertEquals( this.m_rangeWidget.m_minClipText.getValue(), 75 );
            this.assertTrue( this.isWarningPosted( this.m_rangeWidget.m_minClipText));
            this.m_rangeWidget.m_minClipText.setValue( 25 );
            this.assertEquals( this.m_rangeWidget.m_minClipText.getValue(), 25 );
            this.assertFalse( this.isWarningPosted( this.m_rangeWidget.m_minClipText));
        },
        
        /**
         * Test that if a maximum value smaller than a minimum value is set a warning is posted.
         * The warning should then disappear if a larger maximum value is set.
         */
        testTooSmallMaxValue : function() {
            this.m_rangeWidget.m_minClipText.setValue( 75 );
            this.assertEquals( this.m_rangeWidget.m_minClipText.getValue(), 75 );
            this.m_rangeWidget.m_maxClipText.setValue(25);
            this.assertEquals( this.m_rangeWidget.m_maxClipText.getValue(), 25 );
            this.assertTrue( this.isWarningPosted( this.m_rangeWidget.m_maxClipText));
            this.m_rangeWidget.m_maxClipText.setValue( 100 );
            this.assertEquals( this.m_rangeWidget.m_maxClipText.getValue(), 100 );
            this.assertFalse( this.isWarningPosted( this.m_rangeWidget.m_maxClipText));
        },
        

        m_rangeWidget : null

    }
});
