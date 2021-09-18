#ifndef PAGE_COININFOMATION_H
#define PAGE_COININFOMATION_H


//
//   The HTML PAGE
//
const char PAGE_CoinInformation[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="refresh" content="3; url=/coininfo.html?Refresh">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1">
  
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="javascript:GetState()"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Hotspot Insert Coin</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">BAYAD :</td><td><span id="x_magkano"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">DAYS :</td><td><span id="x_days"></span></td></tr>
<tr><td align="right">HOURS :</td><td><span id="x_hours"></span></td></tr>
<tr><td align="right">MINS :</td><td><span id="x_mins"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
	setValues("/admin/coininfo");
}

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				GetState();
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;


//
// FILL WITH INFOMATION
// 

void send_coininfo_values_html ()
{

	String values ="";

	values += "x_magkano|" + (String)xtmp_magkano +  "|div\n";
	values += "x_days|" + (String)xtmp_days +  "|div\n";
	values += "x_hours|" + (String)xtmp_hours +  "|div\n";
	values += "x_mins|" + (String)xtmp_mins +  "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 

}


#endif
