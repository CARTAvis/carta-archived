qx.Class.define("skel.test.ProfileFitTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_fitWidget = new skel.widgets.Profile.SettingsFit();
        },

        tearDown : function() {
            this.m_fitWidget.dispose();
            this.m_fitWidget = null;
        },
        


        /**
         * Test that checking/unchecking manual initial guess enables/disables
         * the manual guesses.
         */
        testManual : function() {
            this.m_fitWidget.setManual( true );
            this.assertTrue( this.m_fitWidget.m_fitDisplay.m_guessCheck.isEnabled() );
            this.m_fitWidget.setManual( false );
            this.assertFalse( this.m_fitWidget.m_fitDisplay.m_guessCheck.isEnabled() );
        },
        
        
        
        m_fitWidget : null

    }
});
