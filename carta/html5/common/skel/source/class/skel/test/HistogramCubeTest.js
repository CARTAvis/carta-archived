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
            this.assertFalse( this.m_rangeWidget.m_rangeMinText.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMaxText.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_unitCombo.isEnabled());
        },
        
        
        /**
         * Test that the single plane widget is disabled and range widgets are
         * enabled in range mode.
         */
        testRangePlaneEnable : function() {
            this.m_rangeWidget.setPlaneMode("Range");
            this.assertTrue( this.m_rangeWidget.m_unitCombo.isEnabled());
            this.assertTrue( this.m_rangeWidget.m_rangeMinText.isEnabled());
            this.assertTrue( this.m_rangeWidget.m_rangeMaxText.isEnabled());
            
        },
        
        /**
         * Test that plane specification widgets are all disabled if we are in
         * entire cube mode.
         */
        testAllPlaneEnable : function() {
            this.m_rangeWidget.setPlaneMode("All");
            this.assertFalse( this.m_rangeWidget.m_unitCombo.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMinText.isEnabled());
            this.assertFalse( this.m_rangeWidget.m_rangeMaxText.isEnabled());
            
        },
        
        /**
         * Test that we can set valid values.
         */
        testRangeValues : function(){
            this.m_rangeWidget.setPlaneMode("Range");
            this.m_rangeWidget.m_rangeMinText.setValue( "0" );
            this.assertEquals( this.m_rangeWidget.m_rangeMinText.getValue(), "0");
            this.m_rangeWidget.m_rangeMaxText.setValue( "10");
            this.assertEquals( this.m_rangeWidget.m_rangeMaxText.getValue(), "10");
        },
        
        /**
         * Test the limit on cube size in the histogram.
         */
        testAllValues : function(){
        	this.m_rangeWidget.setPlaneMode( "All");
        	this.m_rangeWidget.m_smallCheck.setValue( false );
        	this.assertFalse( this.m_rangeWidget.m_pixelText.isEnabled() );
        	this.m_rangeWidget.m_smallCheck.setValue( true );
        	this.assertTrue( this.m_rangeWidget.m_pixelText.isEnabled() );  	
        },
        
        m_rangeWidget : null

    }
});
