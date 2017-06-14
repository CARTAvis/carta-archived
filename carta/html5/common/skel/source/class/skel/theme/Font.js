/* ************************************************************************

   Copyright:

   License:

   Authors:

************************************************************************ */

qx.Theme.define("skel.theme.Font",
{
  extend : qx.theme.modern.Font,

  fonts :
  {
      "default" :
      {
        size : 14,
        family : [ "carta_custom" ],
        sources:
        [
            {
                family : "carta_custom",
                source:
                [
                    "common/skel/source/class/skel/theme/Roboto-Regular.ttf"
                ]
            }
        ]
      },
      "titleFont" :
          {
              include : "default",
              size : 16,
              bold : true
          }
  }
});
