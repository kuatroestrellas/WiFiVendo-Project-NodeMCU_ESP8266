//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="javascript:void(0)" onclick="location.href='admin.html'"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Rates Settings</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
  <td align="center">Vendo Package Rates</td>
  <td><input type="hidden" id="devicename" name="devicename" value=""></td>
</tr>
<tr>
  <td align="left" colspan="2"><hr></td>
</tr>
<tr>
  <td align="left" colspan="2">Convert Your TIME into MINUTES</td>
</tr>
<tr>
  <td align="right">BaseRatePerMinute: e.g 1=10mins</td>
  <td><input type="text" id="RatePerMins" name="RatePerMins" size="2" value="10"></td>
</tr>
<tr>
  <td align="center">PisoRate1 Php :</td>
  <td><input type="text" id="PR1PHP" name="PR1PHP" size="2" value="5"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR1MIN" name="PR1MIN" size="2" value="60"></td>
  <td align="left">HSProf1: </td>
  <td><input type="text" id="HSprof1" name="HSprof1" size="2" value=""></td>
</tr>
<tr>
  <td align="center">PisoRate2 Php :</td>
  <td><input type="text" id="PR2PHP" name="PR2PHP" size="2" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR2MIN" name="PR2MIN" size="2" value="00"></td>
  <td align="left">HSProf2: </td>
  <td><input type="text" id="HSprof2" name="HSprof2" size="2" value=""></td>
</tr>
<tr>
  <td align="center">PisoRate3 Php :</td>
  <td><input type="text" id="PR3PHP" name="PR3PHP" size="3" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR3MIN" name="PR3MIN" size="3" value="00"></td>
  <td align="left">HSProf3: </td>
  <td><input type="text" id="HSprof3" name="HSprof3" size="2" value=""></td>  
</tr>
<tr>
  <td align="center">PisoRate4 Php :</td>
  <td><input type="text" id="PR4PHP" name="PR4PHP" size="3" maxlength="8" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR4MIN" name="PR4MIN" size="3" maxlength="8" value="00"></td>
  <td align="left">HSProf4: </td>
  <td><input type="text" id="HSprof4" name="HSprof4" size="2" value=""></td>
</tr>
<tr>
  <td align="center">PisoRate5 Php :</td>
  <td><input type="text" id="PR5PHP" name="PR5PHP" size="3" maxlength="8" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR5MIN" name="PR5MIN" size="3" maxlength="8" value="00"></td>
  <td align="left">HSProf5: </td>
  <td><input type="text" id="HSprof5" name="HSprof5" size="2" value=""></td>
</tr>
<tr>
  <td align="center">PisoRate6 Php :</td>
  <td><input type="text" id="PR6PHP" name="PR6PHP" size="3" maxlength="8" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR6MIN" name="PR6MIN" size="3" maxlength="8" value="00"></td>
  <td align="left">HSProf6: </td>
  <td><input type="text" id="HSprof6" name="HSprof6" size="2" value=""></td>
</tr>
<tr>
  <td align="center">PisoRate7 Php :</td>
  <td><input type="text" id="PR7PHP" name="PR7PHP" size="3" maxlength="8" value="00"></td>
  <td align="left">Mins: </td>
  <td><input type="text" id="PR7MIN" name="PR7MIN" size="3" maxlength="8" value="00"></td>
  <td align="left">HSProf7: </td>
  <td><input type="text" id="HSprof7" name="HSprof7" size="2" value=""></td>
</tr>
<tr>
  <tr></tr>
<td align="center">Telnet RouterOS Account</td>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr></tr>
  <td align="left">NOTE: Must Manually Create To Mikrotik</td>
  <tr></tr>
<tr></tr>
  <td align="left">NOT Applicable to DEMO Firmware</td>
  <tr></tr>
 <tr></tr>
  <label for="vTelnetUsr">Username :</label>
  <input type="text" id="vTelnetUsr" name="vTelnetUsr" size="30" maxlength="32" value=""><br>
  <label for="vTelnetPwd">Password:</label>
  <input type="text" id="vTelnetPwd" name="vTelnetPwd" size="30" maxlength="32" value=""><br> 
  <tr></tr>
</tr>

<table border="0"  cellspacing="0" cellpadding="3" >
<tr></tr>
<tr><td>Setup Protection</td><td>
<select  id="LockSetup" name="LockSetup">
  <option value="1">AdminProtection = ENABLED</option>
  <option value="0">AdminProtection = DISABLE</option>
</select>
</td></tr>
  
<!--"Sample Rates 5=1H 10=3H 20=10H 30=1D 50=3D 100=7D 300=30D++" -->
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html()
{
	
	if (server.args() > 0 )  // Save Settings
	{
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i));
      if (server.argName(i) == "vTelnetUsr") config.vTelnetUsr = urldecode(server.arg(i));
      if (server.argName(i) == "vTelnetPwd") config.vTelnetPwd = urldecode(server.arg(i)); 
if (server.argName(i) == "RatePerMins") config.vPH =  server.arg(i).toInt();
if (server.argName(i) == "PR1PHP") config.vPH1 =  server.arg(i).toInt();
if (server.argName(i) == "PR1MIN") config.vPK1 =  server.arg(i).toInt();
if (server.argName(i) == "PR2PHP") config.vPH2 =  server.arg(i).toInt();
if (server.argName(i) == "PR2MIN") config.vPK2 =  server.arg(i).toInt();
if (server.argName(i) == "PR3PHP") config.vPH3 =  server.arg(i).toInt();
if (server.argName(i) == "PR3MIN") config.vPK3 =  server.arg(i).toInt();
if (server.argName(i) == "PR4PHP") config.vPH4 =  server.arg(i).toInt();
if (server.argName(i) == "PR4MIN") config.vPK4 =  server.arg(i).toInt();
if (server.argName(i) == "PR5PHP") config.vPH5 =  server.arg(i).toInt();
if (server.argName(i) == "PR5MIN") config.vPK5 =  server.arg(i).toInt();
if (server.argName(i) == "PR6PHP") config.vPH6 =  server.arg(i).toInt();
if (server.argName(i) == "PR6MIN") config.vPK6 =  server.arg(i).toInt();
if (server.argName(i) == "PR7PHP") config.vPH7 =  server.arg(i).toInt();
if (server.argName(i) == "PR7MIN") config.vPK7 =  server.arg(i).toInt();
if (server.argName(i) == "HSprof1") config.vHP1 =  server.arg(i).toInt();
if (server.argName(i) == "HSprof2") config.vHP2 =  server.arg(i).toInt(); 
if (server.argName(i) == "HSprof3") config.vHP3 =  server.arg(i).toInt(); 
if (server.argName(i) == "HSprof4") config.vHP4 =  server.arg(i).toInt(); 
if (server.argName(i) == "HSprof5") config.vHP5 =  server.arg(i).toInt(); 
if (server.argName(i) == "HSprof6") config.vHP6 =  server.arg(i).toInt(); 
if (server.argName(i) == "HSprof7") config.vHP7 =  server.arg(i).toInt();  
if (server.argName(i) == "LockSetup") config.vLockSetup =  server.arg(i).toInt(); 
	}
		WriteConfig();
		firstStart = true;
	}
	server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
	
	
}

void send_general_configuration_values_html()
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
values += "RatePerMins|" +  (String)  config.vPH +  "|input\n";
values += "PR1PHP|" +  (String)  config.vPH1 +  "|input\n";
values += "PR1MIN|" +  (String)  config.vPK1 +  "|input\n";
values += "PR2PHP|" +  (String)  config.vPH2 +  "|input\n";
values += "PR2MIN|" +  (String)  config.vPK2 +  "|input\n";
values += "PR3PHP|" +  (String)  config.vPH3 +  "|input\n";
values += "PR3MIN|" +  (String)  config.vPK3 +  "|input\n";
values += "PR4PHP|" +  (String)  config.vPH4 +  "|input\n";
values += "PR4MIN|" +  (String)  config.vPK4 +  "|input\n";
values += "PR5PHP|" +  (String)  config.vPH5 +  "|input\n";
values += "PR5MIN|" +  (String)  config.vPK5 +  "|input\n";
values += "PR6PHP|" +  (String)  config.vPH6 +  "|input\n";
values += "PR6MIN|" +  (String)  config.vPK6 +  "|input\n";
values += "PR7PHP|" +  (String)  config.vPH7 +  "|input\n";
values += "PR7MIN|" +  (String)  config.vPK7 +  "|input\n";
values += "HSprof1|" +  (String)  config.vHP1 +  "|input\n";
values += "HSprof2|" +  (String)  config.vHP2 +  "|input\n";
values += "HSprof3|" +  (String)  config.vHP3 +  "|input\n";
values += "HSprof4|" +  (String)  config.vHP4 +  "|input\n";
values += "HSprof5|" +  (String)  config.vHP5 +  "|input\n";
values += "HSprof6|" +  (String)  config.vHP6 +  "|input\n";
values += "HSprof7|" +  (String)  config.vHP7 +  "|input\n";
values += "LockSetup|" +  (String) config.vLockSetup + "|input\n";
values += "vTelnetUsr|" + (String) config.vTelnetUsr + "|input\n";
values += "vTelnetPwd|" +  (String) config.vTelnetPwd + "|input\n";
 
///*	values += "toffenabled|" +  (String) (config.AutoTurnOff ? "checked" : "") + "|chk\n";
///*	values += "tonenabled|" +  (String) (config.AutoTurnOn ? "checked" : "") + "|chk\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
