const char index_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<HTML>
  <HEAD>
    <link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAASPU4ADhb9QA49e8AOMP1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAgAAAAARAEAiAAAAABEwRCIAAAAAETNEIgAAAAARM0QiAAAAAAEzRCAAAAAAADNEAAAAAAAAA0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//wAA//8AAOAHAADgBwAA4AcAAOAHAADgBwAAoAUAAIABAADAAwAA4AcAAPAPAAD4HwAA/D8AAP5/AAD//wAA" rel="icon" type="image/x-icon" />
    <META name='viewport' content='width=device-width, initial-scale=1'>
    
    <TITLE>HomeKit Controller</TITLE>

    <link rel="stylesheet" href="style.css?v=36" type="text/css" media="screen" />
    <script>
      var methodending = '';
      var xmlHttpRepository = createXmlHttpObject();
      var xmlHttpPictures = createXmlHttpObject();
      var xmlHttpState = createXmlHttpObject();
      var xmlHttpLogging = createXmlHttpObject();
      var xmlHttpDeviceDelete = createXmlHttpObject();
      var xmlHttpDeviceSave = createXmlHttpObject();
      var xmlHttpBoardSave = createXmlHttpObject();
      var xmlHttpControllerSave = createXmlHttpObject();
      var xmlHttpControllerRestart = createXmlHttpObject();
      var json_obj = "";
      var json_obj_pic = "";
      var json_obj_state = "";
      var json_obj_log = "";

      // Pic Encoding https://base64.guru/converter/encode/file

      var status_controller = '---';
      var status_runtime_sec = '---';
      var status_wifi_rssi = '---';
      var status_heap_free = '---';
      var status_psram_free = '---';

      var abort_reload_site = false;

      function createXmlHttpObject(){
        if(window.XMLHttpRequest){
            xmlHttp=new XMLHttpRequest();
        }else{
            xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
        }
        return xmlHttp;
      }
    </script>
    <script src="java.js?v=36"></script>
    <script src="java_helper.js?v=36"></script>

  </HEAD>
  <BODY onload='actPicturesControllerAndDevices();actState();'>
    <div id="box_message" class="box_message"></div>
    <A id="box_setting_frame"></A>
    <CENTER>
    <br>
    <A id="hub_header" class="hub_header waiting_gray"></A><br>
    <div id="div_hub_status" class="div_hub_status waiting_gray" onclick="openInfo()" style="opacity:0;">
      <A id='hub_status'>&nbsp;</A><br><A id='homekit_code'>&nbsp;</A></td> 
    </div>
    <br>
    <table class="table_main" style="width:600px;height:34px;">
        <tr>
            <td class="table_main_device" style="width:290px;height:26px;"><A id="devices_header" class="devices_header waiting_gray"></A> </td>
            <td class="table_main_loader" style="width:40px;"><div id="loader" class="loader"></div></td>
            <td class="table_main_loadertext" style="width:210px;"><div id='loader_message' class="loader_text"></div></td>
            <td class="table_main_add" style="width:30px;"><div id="div_device_add" class="div_device_add waiting_gray" onclick="openSettings(0)" style="opacity:0;">+</div></td>
            <td class="table_main_reload" style="width:30px;"><div id="div_device_reload" class="div_device_reload waiting_gray" onclick="actControllerAndDevices()" style="opacity:0;">&#8635;</div></td>
        </tr>
    </table>
    <div id="table_devices" class="table_main" style="width:600px;display: inline-block;">
        <A id='response_items'></A>
    </div> 
    <br>
    <div id="table_restart" class="table_main" style="width:600px;text-align:center;">
        <A id='restart_item'></A>
    </div> 
    <br><br><br><br><br>
    <div id="box_footer" class="box_footer"></div>
    </CENTER>
  </BODY>
</HTML>
)=====";
