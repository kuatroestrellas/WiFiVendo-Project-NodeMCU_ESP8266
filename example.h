#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The EXAMPLE PAGE
//
const char PAGE_example[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta http-equiv="refresh" content="1; url=/admin.html"">
  <meta http-equiv="pragma" content="no-cache">
  <meta http-equiv="expires" content="-1">
  <h1>Web Coin Counter</h1>     
    
   <div id="mydynamicdata">For More Details Contact facebook.com/VENDOTRONICS</div>   <!-- added a DIV, where the dynamic data goes to -->
   <script>                


		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
						setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
				});
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



   </script>

)=====";
#endif


void filldynamicdata()
{        
    String values ="";
    values += "mydynamicdata|" + (String) + " VENDOTRONICS | Contact Me : facebook.com/VENDOTRONICS " + (String) + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   
}

void processExample()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", PAGE_example  ); 
}
