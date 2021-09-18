//
//  HTML PAGE
// <form action="/action_page.php">
// <label for="country">Country:</label>
//  <input type="text" id="country" name="country" value="Norway" readonly><br><br>
//  <input type="submit" value="Submit">
// </form>
//

const char PAGE_RebootGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>REBOOT</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td><input type="hidden" id="totalsales" name="totalsales" value="" readonly></td>
</tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="DISCONNECTED"></td></tr>
</table>
</form>
<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/rebootvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_sales_value_html()
{
		
	String values ="";
	values += "totalsales|" + (String) config.vSales + "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_reboot_html()
{
	
	if (server.args() > 0 )  // Save Settings
	{
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			// if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
      if (server.argName(i) == "totalsales") config.vSales =  server.arg(i).toInt();
		}
//		WriteConfig();
//    espSysReboot();
		firstStart = true;
	}
	server.send ( 200, "text/html", PAGE_RebootGeneralSettings ); 
	Serial.println(__FUNCTION__); 
	
	
}

void send_reboot_configuration_values_html()
{
	String values ="";
///*	values += "totalsales|" +  (String)  config.vSales +  "|input\n";
///*	values += "tonhour|" +  (String)  config.TurnOnHour +  "|input\n";
///*	values += "tonminute|" +   (String) config.TurnOnMinute +  "|input\n";
///*	values += "toffhour|" +  (String)  config.TurnOffHour +  "|input\n";
///*	values += "toffminute|" +   (String)  config.TurnOffMinute +  "|input\n";
///*	values += "toffenabled|" +  (String) (config.AutoTurnOff ? "checked" : "") + "|chk\n";
///*	values += "tonenabled|" +  (String) (config.AutoTurnOn ? "checked" : "") + "|chk\n";
  espSysReboot();
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
