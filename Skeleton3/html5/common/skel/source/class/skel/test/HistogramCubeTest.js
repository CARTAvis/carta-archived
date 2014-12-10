qx.Class.define("skel.test.HistogramCubeTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_rangeWidget = new skel.widgets.Histogram.HistogramCube();
        },

        tearDown : function() {
            this.m_rangeWidget.dispose();
            this.m_rangeWidget = null;
        },
        


        /**
         * Test that if single plane is selected, the single plane text field is enabled and range spins are disabled..
         */
        testSinglePlaneEnable : function() {
            this.m_rangeWidget.setPlaneMode("Single");
            this.assertTrue( this.m_rangeWidget.m_singlePlaneText.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMinSpin.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMaxSpin.isEnabled());
        },
        
        /**
         * Test that we can enter nonnegative values, but there is a warning if negative values are entered.
         */
        testSinglePlaneValues : function(){
            this.m_rangeWidget.setPlaneMode("Single");
            this.m_rangeWidget.m_singlePlaneText.setValue( 50 );
            this.assertEquals( this.m_rangeWidget.m_singlePlaneText.getValue(), 50);
            this.m_rangeWidget.m_singlePlaneText.setValue( 0 );
            this.assertEquals( this.m_rangeWidget.m_singlePlaneText.getValue(), 0 );
            this.m_rangeWidget.m_singlePlaneText.setValue( -1 );
            this.assertEquals( this.m_rangeWidget.m_singlePlaneText.getValue(), -1 );
            var warningIndex = this.m_rangeWidget.m_singlePlaneText.indexOf( this.m_rangeWidget.m_singlePlaneText.m_warning );
            var warningPosted = false;
            if ( warningIndex >= 0 ){
                warningPosted = true;
            }
            this.assertTrue( warningPosted );
        },
        
        /**
         * Test that the single plane widget is disabled and range widgets are
         * enabled in range mode.
         */
        testRangePlaneEnable : function() {
            this.m_rangeWidget.setPlaneMode("Range");
            this.assertFalse( this.m_rangeWidget.m_singlePlaneText.isEnabled());
            this.assertTrue( this.m_rangeWidget.m_rangeMinSpin.isEnabled());
            this.assertTrue( this.m_rangeWidget.m_rangeMaxSpin.isEnabled());
        },
        
        /**
         * Test that plane specification widgets are all disabled if we are in
         * entire cube modee.
         */
        testAllPlaneEnable : function() {
            this.m_rangeWidget.setPlaneMode("All");
            this.assertFalse( this.m_rangeWidget.m_singlePlaneText.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMinSpin.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMaxSpin.isEnabled());
        },
        /**
         * Test that we cannot set a negative value, test that we can set valid values,
         * test that the minimum cannot be set to a value larger than the maximum.
         */
        testRangeValues : function(){
            this.m_rangeWidget.setPlaneMode("Range");
            this.m_rangeWidget.m_rangeMinSpin.setValue( 0 );
            this.assertEquals( this.m_rangeWidget.m_rangeMinSpin.getValue(), 0);
            this.m_rangeWidget.m_rangeMinSpin.setValue( -2 );
            this.assertEquals( this.m_rangeWidget.m_rangeMinSpin.getValue(), 0);
            this.m_rangeWidget.m_rangeMaxSpin.setValue( 10 );
            this.assertEquals( this.m_rangeWidget.m_rangeMaxSpin.getValue(), 10);
            this.m_rangeWidget.m_rangeMinSpin.setValue( 10 );
            this.assertEquals( this.m_rangeWidget.m_rangeMinSpin.getValue(), 10);
            this.m_rangeWidget.m_rangeMinSpin.setValue( 11 );
            this.assertEquals( this.m_rangeWidget.m_rangeMinSpin.getValue(), 10);
        },
        m_rangeWidget : null

    }
});
