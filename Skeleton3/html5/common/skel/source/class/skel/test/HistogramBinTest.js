qx.Class.define("skel.test.HistogramCubeTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_binWidget = new skel.widgets.Histogram.HistogramBin();
        },

        tearDown : function() {
            this.m_binWidget.dispose();
            this.m_binWidget = null;
        },
        


        /**
         * Test that if we change the text field value, the slider value moves to the same value.
         */
        testTextToSlider : function() {
            this.m_binWidget.setBinCount( 35);
            this.assertTrue( this.m_binWidget.m_binText.getValue(), 35);
            this.assertTrue( this.m_binWidget.m_binSlider.getValue(), 35);
        },
        

        m_binWidget : null

    }
});
