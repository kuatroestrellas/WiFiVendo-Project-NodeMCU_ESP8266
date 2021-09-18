

//
//  HTML PAGE
//  <a href="javascript:void(0)" onclick="location.href='http://www.google.com'">Hide Link</a>


const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<strong>Vendo Configuration</strong>
<hr>
<a href="javascript:void(0)" onclick="location.href='config.html'" style="width:250px" class="btn btn--m btn--blue" >My Wireless Name</a><br>
<a href="javascript:void(0)" onclick="location.href='ntp.html'"   style="width:250px"  class="btn btn--m btn--blue" >NTP Clock Settings</a><br>
<a href="javascript:void(0)" onclick="location.href='general.html'" style="width:250px" class="btn btn--m btn--blue" >Vendo Promo Rates</a><br>
<a href="javascript:void(0)" onclick="location.href='info.html'"   style="width:250px"  class="btn btn--m btn--blue" >Device Information</a><br>
<a href="javascript:void(0)" onclick="location.href='sysreboot.html'"   style="width:250px"  class="btn btn--m btn--blue" >ReStart Device</a><br>

<script>
window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				// Do something after load...
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";
