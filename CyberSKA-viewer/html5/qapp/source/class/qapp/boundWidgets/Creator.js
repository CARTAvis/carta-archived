/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 11/11/13
 * Time: 12:32 AM
 * To change this template use File | Settings | File Templates.
 */

qx.Class.define("qapp.boundWidgets.Creator",
    {
        statics :
        {
            createCheckBox : function( label, path) {
                var res = new qapp.boundWidgets.CheckBox( label, path);
                return res;
            }
        }
    });
