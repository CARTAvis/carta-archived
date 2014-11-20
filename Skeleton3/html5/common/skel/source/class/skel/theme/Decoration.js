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
      "focused-inset" :
      {
        style :
        {
          width : 1,
          innerWidth: 1,
          color : [ "border-focused-dark-shadow", "border-focused-light", "border-focused-light", "border-focused-dark-shadow" ],
          innerColor : [ "border-focused-dark", "border-focused-light-shadow", "border-focused-light-shadow", "border-focused-dark" ]
        }
      },
	  
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
        
        "slider-red" :
        {
           style :
            {          
                backgroundColor : "#FF0000"
            }
         },
         
         "slider-blue" :
         {
            style :
             {          
                 backgroundColor : "#0000FF"
             }
          },
          
          "slider-green" :
          {
             style :
              {          
                  backgroundColor : "#00FF00"
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