/* ************************************************************************

   Copyright:

   License:

   Authors:

************************************************************************ */

qx.Theme.define("skel.theme.Decoration",
{
  extend : qx.theme.modern.Decoration,

  decorations :
  {
	 
	  
	  "light-border" :
      {
          style :
          {
              width : 5,
              radius: 3,
              color : "#000000"
          }
      },
      
      "line-border" :
      {
          style :
          {
              width : 2,
              radius: 1,
              color : "#000000"
          }
      },

      "no-border" :
      {
          style :
          {
              width : 0
        
          }
      },
      
      "status-bar" :
      {
    	  style :
          {          
              backgroundColor : "background"
          }
       },
       
       "popup-dialog" :
       {
     	  style :
           {          
               backgroundColor : "background"
           }
        },
       
       "win" :
       {
     	  style :
           {
               backgroundColor : "background",
               width : 0
           }
        },
        
        "desktop" :
        {
      	  style :
            {
               
                backgroundColor : "background"
            }
         },
        
        "window-pane-active" :
        {
      	  style :
            {       
                backgroundColor : "background-pane",
                width : 5,
                color : "selection"
            }
         }
  }
});