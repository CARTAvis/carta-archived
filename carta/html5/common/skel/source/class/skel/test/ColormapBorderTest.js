qx.Class.define("skel.test.ColormapBorderTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_borderWidget = new skel.widgets.Colormap.PageBorderBackground();
        },

        tearDown : function() {
            this.m_borderWidget.dispose();
            this.m_borderWidget = null;
        },
        


        /**
         * Test that enabling/disabling the default border changes the enabled
         * status of the customization widgets appropriately.
         */
        testDefaultEnable : function() {
            this.m_borderWidget._setBorderDefault( false );
            this.assertTrue( this.m_borderWidget.m_transparency.isEnabled());
            this.assertTrue( this.m_borderWidget.m_colorSelector.isEnabled());
            this.m_borderWidget._setBorderDefault( true );
            this.assertFalse( this.m_borderWidget.m_transparency.isEnabled());
            this.assertFalse( this.m_borderWidget.m_colorSelector.isEnabled());
        },
        
        
        /**
         * Test that the opacity cannot be set to a value less than zero.
         */
        testOpacityMin : function() {
            this.m_borderWidget._setBorderDefault( false );
            this.m_borderWidget._setTransparency( -10 );
            this.assertTrue( this.m_borderWidget.m_transparency._isWarning());
            this.m_borderWidget._setTransparency( 10 );
            this.assertFalse( this.m_borderWidget.m_transparency._isWarning());
            
        },
        
        /**
         * Test that the opacity cannot be set to a value larger than 255.
         */
        testOpacityMax : function() {
        	this.m_borderWidget._setBorderDefault( false );
            this.m_borderWidget._setTransparency( -10 );
            this.assertTrue( this.m_borderWidget.m_transparency._isWarning());
            this.m_borderWidget._setTransparency( 10 );
            this.assertFalse( this.m_borderWidget.m_transparency._isWarning());  
        },
        
        m_rangeWidget : null
    }
});
