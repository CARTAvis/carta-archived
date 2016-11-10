qx.Class.define("skel.test.GridGridTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_gridPage = new skel.widgets.Image.Grid.Settings.SettingsGridPage();
        },

        tearDown : function() {
            this.m_gridPage.dispose();
            this.m_gridPage = null;
        },
        

        /**
         * Test checking/unchecking the grid lines check box changes the enabled
         * status of the controls.
         */
        testEnableLines : function() {
            this.m_gridPage._setShowGridLines( false );
            this.assertFalse( this.m_gridPage.m_thickness.isEnabled());
            this.assertFalse( this.m_gridPage.m_transparency.isEnabled());
            this.assertFalse( this.m_gridPage.m_spacing.isEnabled());
            this.m_gridPage._setShowGridLines( true );
            this.assertTrue( this.m_gridPage.m_thickness.isEnabled());
            this.assertTrue( this.m_gridPage.m_transparency.isEnabled());
            this.assertTrue( this.m_gridPage.m_spacing.isEnabled());
        },
        
        /**
         * Test that grid line transparency cannot be set to a value less than 0.
         */
        testTransparencyMin : function(){
        	this.m_gridPage._setShowGridLines( true );
        	this.m_gridPage._setTransparency( -10 );
        	var error = this.m_gridPage.m_transparency._isWarning();
        	this.assertTrue( error )
        	this.m_gridPage._setTransparency( 10 );
        	this.assertFalse( this.m_gridPage.m_transparency._isWarning());
        },
        
        /**
         * Test that a warning will be posted if the opacity is set above 255.
         */
        testTransparencyMax : function(){
        	this.m_gridPage._setShowGridLines( true );
        	this.m_gridPage._setTransparency( 300 );
        	var error = this.m_gridPage.m_transparency._isWarning();
        	this.assertTrue( error )
        	this.m_gridPage._setTransparency( 200 );
        	this.assertFalse( this.m_gridPage.m_transparency._isWarning());
        },
        
        /**
         * Test that a warning will be posted if the thickness is set to less than 0.
         */
        testThicknessMin : function(){
        	this.m_gridPage._setShowGridLines( true );
        	this.m_gridPage._setThickness( -10 );
        	var error = this.m_gridPage.m_thickness._isWarning();
        	this.assertTrue( error )
        	this.m_gridPage._setThickness( 10 );
        	this.assertFalse( this.m_gridPage.m_thickness._isWarning());
        },
        
      
        

        m_gridPage : null

    }
});
