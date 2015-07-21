qx.Class.define("skel.test.NumericTextFieldTest", {
    extend : qx.dev.unit.TestCase,

    members : {
        /*
         * ---------------------------------------------------------------------------
         * TESTS
         * ---------------------------------------------------------------------------
         */

        setUp : function() {
            this.m_numeric = new skel.widgets.CustomUI.NumericTextField(0, 100);
        },

        tearDown : function() {
            this.m_numeric.dispose();
            this.m_numeric = null;
        },
        
        /**
         * Returns whether or not a warning label is posted.
         */
        isWarning : function(){
            var warningPosted = false;
            var warningIndex = this.m_numeric.indexOf( this.m_numeric.m_warning );
            if ( warningIndex >= 0 ){
                warningPosted = true;
            }
            return warningPosted;
        },

        /**
         * Test that a too large value results in a warning although the value is set.
         */
        testTooLarge : function() {
            this.m_numeric.setValue( 50 );
            this.assertEquals( this.m_numeric.m_text.getValue(), "50");
            this.m_numeric.setValue(200);
            this.assert( this.isWarning(), "Warning not shown when setting a value too large." );
            this.assertEquals( this.m_numeric.m_text.getValue(), "200");
        },
        
        /**
         * Test that a too small value results in a warning although the value is set.
         */
        testTooSmall : function(){
            this.m_numeric.setValue( 50 );
            this.assertEquals( this.m_numeric.m_text.getValue(), "50");
            this.m_numeric.setValue(-1);
            this.assert( this.isWarning(), "Warning not shown when setting a value too small." );
            this.assertEquals( this.m_numeric.m_text.getValue(), "-1");
        },
        
        /**
         * Test that an integer only numeric text field ignores attempts to set floats.
         */
        testNoFloats : function(){
            this.m_numeric.setValue( 25 );
            this.m_numeric.setValue(3.234);
            this.assertEquals( this.m_numeric.m_text.getValue(), "25");
        },
        
        /**
         * Test that values within the preset bounds are correctly placed in the text field.
         */
        testAcceptable : function(){
            this.m_numeric.setValue( 25 );
            this.assert( !this.isWarning(), "Valid value generating a warning!");
            this.assertEquals( this.m_numeric.m_text.getValue(), "25");
        },
        
        /**
         * Test that string values are ignored.
         */
        testNumeric : function(){
            this.m_numeric.setValue( 25 );
            this.m_numeric.setValue( "Hi");
            this.assertEquals( this.m_numeric.m_text.getValue(), "25");
        },
        
        m_numeric : null

    }
});
