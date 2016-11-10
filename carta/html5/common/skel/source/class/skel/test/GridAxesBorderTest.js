qx.Class.define("skel.test.GridAxesBorderTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_axesPage = new skel.widgets.Image.Grid.Settings.SettingsAxesPage();
        },

        tearDown : function() {
            this.m_axesPage.dispose();
            this.m_axesPage = null;
        },
        


        /**
         * Test that if we disable the axes/border then thickness and opacity cannot
         * be set.
         */
        testDisableAxes : function() {
            this.m_axesPage._setShowAxes( false );
            this.assertFalse( this.m_axesPage.m_thickness.isEnabled());
            this.assertFalse( this.m_axesPage.m_transparency.isEnabled());
        },
        
        /**
         * Test that a warning will be posted if the opacity is set to less
         * than zero.
         */
        testOpacityMin : function(){
        	this.m_axesPage._setTransparency( -10 );
        	var error = this.m_axesPage.m_transparency._isWarning();
        	this.assertTrue( error )
        },
        
        /**
         * Test that a warning will be posted if the opacity is set above 255.
         */
        testOpacityMax : function(){
        	this.m_axesPage._setTransparency( 300 );
        	var error = this.m_axesPage.m_transparency._isWarning();
        	this.assertTrue( error )
        },
        
        /**
         * Test that a warning will be posted if the thickness is set less than 0.
         */
        testThicknessMin : function(){
        	this.m_axesPage._setThickness( -10 );
        	var error = this.m_axesPage.m_thickness._isWarning();
        	this.assertTrue( error );
        },
        

        m_axesPage : null

    }
});
