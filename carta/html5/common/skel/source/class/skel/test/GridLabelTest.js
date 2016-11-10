qx.Class.define("skel.test.GridLabelTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_labelPage = new skel.widgets.Image.Grid.Settings.SettingsLabelPage();
        },

        tearDown : function() {
            this.m_labelPage.dispose();
            this.m_labelPage = null;
        },
        

        /**
         * Test that we cannot set the font size to a negative value.
         */
        testFontSizeNegative : function() {
            this.m_labelPage.setFontSize( -10 );
            var fontSize = this.m_labelPage.m_fontSizeSpin.getValue();
            this.assertTrue( fontSize >= 0 );
            this.m_labelPage.setFontSize( 5 );
            fontSize = this.m_labelPage.m_fontSizeSpin.getValue();
            this.assertTrue( fontSize == 5 );
        },
        
        /**
         * Test that we cannot set the precision to a negative value.
         */
        testPrecisionNegative : function(){
        	this.m_labelPage.setPrecision( -10 );
        	var precision = this.m_labelPage.m_decimalSpin.getValue();
        	this.assertTrue( precision >= 0 );
        	this.m_labelPage.setPrecision( 3 );
        	precision = this.m_labelPage.m_decimalSpin.getValue();
        	this.assertTrue( precision == 3 );
        },
        
        m_labelPage : null

    }
});
