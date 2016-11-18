qx.Class.define("skel.test.GridTicksTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_ticksPage = new skel.widgets.Image.Grid.Settings.SettingsTicksPage();
        },

        tearDown : function() {
            this.m_ticksPage.dispose();
            this.m_ticksPage = null;
        },
        
        /**
         * Test that changing the enabled status of the ticks check enables/disables
         * the appropriate custom controls.
         */
        testTicksChangeEnable : function(){
        	this.m_ticksPage._setShowTicks( false );
        	this.assertFalse( this.m_ticksPage.m_length.isEnabled());
        	this.assertFalse( this.m_ticksPage.m_transparency.isEnabled());
        	this.assertFalse( this.m_ticksPage.m_thickness.isEnabled());
        	this.m_ticksPage._setShowTicks( true );
        	this.assertTrue( this.m_ticksPage.m_length.isEnabled());
        	this.assertTrue( this.m_ticksPage.m_transparency.isEnabled());
        	this.assertTrue( this.m_ticksPage.m_thickness.isEnabled());
        },

        /**
         * Test that we cannot set the length to a negative value
         */
        testLengthNegative : function() {
            this.m_ticksPage._setLength( -10 );
            this.assertTrue( this.m_ticksPage.m_length._isWarning() );
            this.m_ticksPage._setLength( 10 );
            this.assertFalse( this.m_ticksPage.m_length._isWarning() );
        },
        
        /**
         * Test that we cannot set the thickness to a negative value.
         */
        testThicknessNegative : function(){
        	this.m_ticksPage._setThickness( -10 );
        	this.assertTrue( this.m_ticksPage.m_thickness._isWarning() );
        	this.m_ticksPage._setThickness( 3 );
        	this.assertFalse( this.m_ticksPage.m_thickness._isWarning() );
        },
        
        /**
         * Test that we cannot set the opacity to a negative value.
         */
        testOpacityMin : function(){
        	this.m_ticksPage._setTransparency( -10 );
        	this.assertTrue( this.m_ticksPage.m_transparency._isWarning() );
        	this.m_ticksPage._setTransparency( 50 );
        	this.assertFalse( this.m_ticksPage.m_transparency._isWarning() );
        },
        
        /**
         * Test that we cannot set the opacity to larger than 255.
         */
        testOpacityMax : function(){
        	this.m_ticksPage._setTransparency( 300 );
        	this.assertTrue( this.m_ticksPage.m_transparency._isWarning() );
        	this.m_ticksPage._setTransparency( 50 );
        	this.assertFalse( this.m_ticksPage.m_transparency._isWarning() );
        },
        
        m_ticksPage : null

    }
});
