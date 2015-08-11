qx.Class.define("skel.test.HistogramBinTest", {
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
            var textValue = this.m_binWidget.m_binCountText.getValue();
            this.assertEquals( this.m_binWidget.m_binCountText.getValue(), 35);
            this.assertEquals( this.m_binWidget.m_binCountSlider.getValue(), 35);
        },
        

        m_binWidget : null

    }
});
