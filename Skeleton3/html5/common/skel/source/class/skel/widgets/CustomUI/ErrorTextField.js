/**
 * Textfield that indicates when the contents hold incorrect values.
 */


qx.Class.define( "skel.widgets.CustomUI.ErrorTextField",
    {
        extend: qx.ui.form.TextField,

        /**
         * Constructor.
         */
        construct: function ( ) {
            this.base(arguments);
            this.m_inError = false;
        },

        members: {
            /**
             * Returns true if there is an error in the text field; false otherwise.
             * @return {boolean} true if there is an error; false otherwise.
             */
            isError : function(){
                return this.m_inError;
            },
            
            /**
             * Returns true if the passed in value is in bounds; otherwise returns false and posts an error.
             * @param num {Number} a value to check.
             * @return {boolean} true if the value is valid; false otherwise.
             */
            setError: function ( inError ) {
                this.m_inError = inError;
                if ( this.m_inError ){
                    this.setAppearance ("errorTextField");
                }
                else {
                    this.setAppearance( "textfield");
                }
            },
            
            m_inError : null
        }

    } );

