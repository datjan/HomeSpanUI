const char index_java[] PROGMEM = R"=====(
function actPicturesControllerAndDevices(){
    showWaiting('pictures');
    if(xmlHttpPictures.readyState==0 || xmlHttpPictures.readyState==4){
        xmlHttpPictures.open('GET','pictures'+methodending,true);
        xmlHttpPictures.onreadystatechange=handleServerResponsePictures;
        xmlHttpPictures.send(null);
    }
}
function actControllerAndDevices(){
    showWaiting('devices');
    if(xmlHttpRepository.readyState==0 || xmlHttpRepository.readyState==4){
        xmlHttpRepository.open('GET','rest'+methodending,true);
        xmlHttpRepository.onreadystatechange=handleServerResponse;
        xmlHttpRepository.send(null);
    }
}
function actLogging(){
    if(xmlHttpLogging.readyState==0 || xmlHttpLogging.readyState==4){
        xmlHttpLogging.open('GET','logging'+methodending,true);
        xmlHttpLogging.onreadystatechange=handleServerResponseLogging;
        xmlHttpLogging.send(null);
    }
}
const actState = async () => {
    if(xmlHttpState.readyState==0 || xmlHttpState.readyState==4){
        xmlHttpState.open('GET','state'+methodending,true);
        xmlHttpState.onreadystatechange=handleServerResponseState;
        xmlHttpState.send(null);
    }
    setTimeout('actState()',10000);
}
const handleServerResponseState = async () => {
    if(xmlHttpState.readyState==4 && xmlHttpState.status==200){
        try {
            json_obj_state = JSON.parse(xmlHttpState.response);
        } catch (e) {
            hideWaiting();
            showError("State JSON response invalid");
            return false;
        }
        // Use global because of async state call
        // Runtime sec
        status_runtime_sec = json_obj_state.runtime_sec;
        if (document.getElementById('runtime_sec')) FadeOutSetContentFadeIn('runtime_sec','<span class="info_text_light">' + status_runtime_sec + ' sec</span>');
        // status_wifi_rssi
        status_wifi_rssi = json_obj_state.wifi_rssi;
        if (document.getElementById('wifi_rssi')) FadeOutSetContentFadeIn('wifi_rssi','<span class="info_text_light">' + status_wifi_rssi + ' dBm</span>');
        // status_heap_free
        status_heap_free = json_obj_state.heap_free;
        if (document.getElementById('heap_free')) FadeOutSetContentFadeIn('heap_free','<span class="info_text_light">' + status_heap_free + ' bytes</span>');
        // status_psram_free
        status_psram_free = json_obj_state.psram_free;
        if (document.getElementById('psram_free')) FadeOutSetContentFadeIn('psram_free','<span class="info_text_light">' + status_psram_free + ' bytes</span>');
        // status_mqtts
        status_mqtt = json_obj_state.mqtt_status;
        if (document.getElementById('mqtt_status')) FadeOutSetContentFadeIn('mqtt_status','<span class="info_text_light">' + status_mqtt + '</span>');
        // Controller Status
        if (status_controller!=json_obj_state.status) { // Only when changed
            status_controller = json_obj_state.status;
            if (document.getElementById('controller_status')) FadeOutSetContentFadeIn('controller_status','<span class="info_text_light">' + status_controller + '</span>');
            if (document.getElementById('hub_status')) FadeOutSetContentFadeIn('hub_status',status_controller);
        }
        // Device State
        for (var i = 0; i < json_obj_state.devices_state.length; i++){
            var obj = json_obj_state.devices_state[i];
            var element_name_id = "device_name_" + obj.id;
            var element_state_id = "device_state_" + obj.id;
            var element_device_id = "div_device_" + obj.id;
            if (document.getElementById(element_state_id)) {
                if (document.getElementById(element_state_id).innerHTML!=obj.state) {
                    FadeOutSetContentFadeIn(element_state_id,obj.state);
                }
                if (obj.marked==true) {
                    document.getElementById(element_device_id).classList.add("div_device_marked");
                    document.getElementById(element_state_id).classList.add("span_device_comment_marked");
                    document.getElementById(element_name_id).classList.add("span_device_name_marked");
                } 
                if (obj.marked==false) {
                    document.getElementById(element_device_id).classList.remove("div_device_marked");
                    document.getElementById(element_state_id).classList.remove("span_device_comment_marked");
                    document.getElementById(element_name_id).classList.remove("span_device_name_marked");
                }
            } 
        }        
    }
}
function handleServerResponsePictures(){
    if(xmlHttpPictures.readyState==4 && xmlHttpPictures.status==200){
        try {
            json_obj_pic = JSON.parse(xmlHttpPictures.response);
        } catch (e) {
            hideWaiting();
            showError("Picture JSON response invalid");
            return false;
        } 
        actControllerAndDevices();     
    }
}
function handleServerResponse(){
    if(xmlHttpRepository.readyState==4 && xmlHttpRepository.status==200){
        try {
            json_obj = JSON.parse(xmlHttpRepository.response);
        } catch (e) {
            hideWaiting();
            showError("Main JSON response invalid",true);
            return false;
        }
        // Fill Hub Header
        document.title = json_obj.homekit.name;
        document.getElementById('hub_header').style.opacity = 0;
        document.getElementById('hub_header').innerHTML = json_obj.homekit.name;
        FadeIn('hub_header');
        // Fill Hub Status
        document.getElementById('hub_status').innerHTML = json_obj.status;
        document.getElementById('homekit_code').innerHTML = json_obj.homekit.code.toString().substring(0, 3) + "-" + json_obj.homekit.code.toString().substring(3, 5) + "-" + json_obj.homekit.code.toString().substring(5, 8);
        FadeIn("div_hub_status");
        // Fill Devices Header
        document.getElementById('devices_header').style.opacity = 0;
        document.getElementById('devices_header').innerHTML = "Devices";
        FadeIn('devices_header');
        FadeIn('div_device_add');
        FadeIn('div_device_reload');
        // Fill QR-Code
        document.getElementById('qrcode').style.opacity = 0;
        document.getElementById('qrcode').innerHTML = "";
        generateQrCode(json_obj.homekit.qrcode);
        FadeIn('qrcode');
        // Fill Hub Footer
        document.getElementById('box_footer').style.opacity = 0;
        document.getElementById('box_footer').innerHTML = '<a href="https://www.homekitblogger.de/" target="_blank" class="box_footer_link">homekitblogger.de</a> | ' + json_obj.homekit.type + " | v" + json_obj.version;
        FadeIn('box_footer');

        var result_text = "";
        for (var i = 0; i < json_obj.homekit_devices.length; i++){
            var obj = json_obj.homekit_devices[i];
            if (obj.active) {
                var pic = "";
                for (var x = 0; x < json_obj_pic.length; x++){
                    if (json_obj_pic[x].id==obj.type.picture_id) pic = '<img width="30px height="30px" style="margin-top:5px;" src="data:' + json_obj_pic[x].data + ',' + json_obj_pic[x].base64 + '">';
                }
                result_text += "<div id=\"div_device_" + obj.id + "\" class=\"div_device waiting_gray\" onclick=\"openSettings(" + obj.id + ")\">";
                result_text += "<table height=\"30px\" class=\"div_device_nobopama\">";
                result_text += "  <tr class=\"div_device_nobopama\">";
                result_text += "    <td class=\"div_device_nobopama\" width=\"30px\"><div id=\"div_device_logo\" class=\"div_device_logo\">" + pic + "</div></td>";
                result_text += "    <td class=\"div_device_nobopama\" width=\"10px\"></td>";
                result_text += "    <td class=\"div_device_nobopama\" width=\"200px\">";
                result_text += "      <span id=\"device_name_" + obj.id + "\" class=\"span_device_name\">" + obj.name + "</span><br>";
                if (obj.restartrequired) {
                    result_text += "<span id=\"device_state_red_" + obj.id + "\" class=\"span_device_comment_red\">restart required</span>";
                    showRestart();
                }
                else result_text += "      <span id=\"device_state_" + obj.id + "\" class=\"span_device_comment\">refreshing...</span>";
                result_text += "    </td>";
                result_text += "  </tr>";
                result_text += "</table>";
                result_text += "</div>";
            }
        }
        result_text += "";
        hideWaiting();
        document.getElementById('table_devices').style.opacity = 0;
        document.getElementById('response_items').innerHTML = result_text;
        FadeIn('table_devices');
    }
}
function handleServerResponseLogging(){
    if(xmlHttpLogging.readyState==4 && xmlHttpLogging.status==200){
        try {
            json_obj_log = JSON.parse(xmlHttpLogging.response);
        } catch (e) {
            hideWaiting();
            showError("Logging JSON response invalid",false);
            return false;
        }
        for (var i = 0; i < json_obj_log.log.length; i++){
            var obj = json_obj_log.log[i];
            if (obj.devicename!="" && document.getElementById('loga_'+i)) {
                document.getElementById('loga_'+i).style.opacity = 0;
                document.getElementById('loga_'+i).innerHTML = (obj.runtime_sec).toFixed(0) + ' - ' + obj.devicename;
                FadeIn('loga_'+i);
            }
            if (obj.devicename!="" && document.getElementById('logb_'+i)) {
                document.getElementById('logb_'+i).style.opacity = 0;
                document.getElementById('logb_'+i).innerHTML = "- " + obj.action;
                FadeIn('logb_'+i);
            }
        } 
    }
}

function openInfo(view_type = "homekit",fade = true) {
    grayOut();
    var result_text = "";
    result_text += "<div id=\"box_settings\" class=\"box_settings\">";
    result_text += "";
    result_text += "<table width=\"100%\">";
    result_text += "<tr><td class=\"box_header\">";
    if (view_type=="homekit") result_text += "<span style=\"cursor:pointer;\" onclick=\"openInfo('homekit',false)\">HK</span>"; 
    else result_text += "<span style=\"cursor:pointer;color:gray;\" onclick=\"openInfo('homekit',false)\">HK</span>";
    result_text += " | ";
    if (view_type=="info") result_text += "<span style=\"cursor:pointer;\" onclick=\"openInfo('info',false)\">Con</span>"; 
    else result_text += "<span style=\"cursor:pointer;color:gray;\" onclick=\"openInfo('info',false)\">Con</span>";
    result_text += " | ";
    if (view_type=="pins") result_text += "<span style=\"cursor:pointer;\" onclick=\"openInfo('pins',false)\">Pins</span>"; 
    else result_text += "<span style=\"cursor:pointer;color:gray;\" onclick=\"openInfo('pins',false)\">Pins</span>";
    result_text += " | ";
    if (view_type=="mqtt") result_text += "<span style=\"cursor:pointer;\" onclick=\"openInfo('mqtt',false)\">Mqtt</span>"; 
    else result_text += "<span style=\"cursor:pointer;color:gray;\" onclick=\"openInfo('mqtt',false)\">Mqtt</span>";
    result_text += " | ";
    if (view_type=="logging") result_text += "<span style=\"cursor:pointer;\" onclick=\"openInfo('logging',false)\">Log</span>"; 
    else result_text += "<span style=\"cursor:pointer;color:gray;\" onclick=\"openInfo('logging',false)\">Log</span>";
    result_text += "</td></tr>";
    result_text += "<tr><td style=\"height:5px;\"></td></tr>";

    // HOMEKIT
    if (view_type=="homekit") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Version</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.version + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:25%;\"><span class=\"info_text\">Status</span></td><td style=\"text-align:right;\"><div id=\"controller_status\"><span class=\"info_text_light\">" + status_controller + "</span></div></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">Runtime</span></td><td style=\"text-align:right;\"><div id=\"runtime_sec\"><span class=\"info_text_light\">" + status_runtime_sec + " sec</span></div></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:20%;\"><span class=\"info_text\">Name</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"controller_homekit_name\" class=\"input_text_light\" size=\"18\" maxlength=\"18\" value=\"" + json_obj.homekit.name + "\">";
        result_text += "        </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Mode</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
                // Type
                result_text += "    <select id='controller_homekit_type'>";
                if (json_obj.homekit.type=="device") result_text += "<option value=\"device\" selected>device</option><option value=\"bridge\">bridge</option>";
                if (json_obj.homekit.type=="bridge") result_text += "<option value=\"bridge\" selected>bridge</option><option value=\"device\">device</option>";
                result_text += "    </select>";
        result_text += "        </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">Devices</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.homekit.activedevices + " of " + json_obj.homekit.maximumdevices + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Code</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"controller_homekit_code\" class=\"input_text_light\" size=\"8\" maxlength=\"8\" value=\"" + json_obj.homekit.code + "\" onkeypress=\"return charInt(event.charCode)\">";
        result_text += "        </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Port</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"controller_homekit_port\" class=\"input_text_light\" size=\"5\" maxlength=\"5\" value=\"" + json_obj.homekit.port + "\" onkeypress=\"return charInt(event.charCode)\">";
        result_text += "        </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:20%;\"><span class=\"info_text\">Host</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.homekit.hostname + "</span></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"saveController()\">Save (unpair and restart)</span><br>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"unpairController();\">Unpair</span> | <span class=\"button_text\" onclick=\"restartController();\">Restart</span>";
        result_text += "  | <span class=\"button_text\" onclick=\"resetController();\">Reset</span> | <span class=\"button_text\" onclick=\"openInfo('wifi',false)\">WiFi</span>";
        result_text += "</div></td></tr>";
    }

    // LOGGING
    if (view_type=="logging") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"info_text_italic\">Seconds since start of controller...</span>";
        for (var i = 0; i < 7; i++){
            result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
            result_text += "    <span id=\"loga_" + i + "\" class=\"info_text\"></span><br>";
            result_text += "    <span id=\"logb_" + i + "\" class=\"info_text_light\"></span>";
        }
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
        actLogging();
    }

    // Info
    if (view_type=="info") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Controller</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.chip_model + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:25%;\"><span class=\"info_text\">Revision</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.chip_revision + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">Core</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.chip_core + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Flash</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.chip_flash + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">HEAP total</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.heap_total + " bytes</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">HEAP free</span></td><td style=\"text-align:right;\"><div id=\"heap_free\"><span class=\"info_text_light\">" + status_heap_free + " bytes</span></div></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">PSRAM found</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + Boolean(Number(json_obj.controller.psram_found))  + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">PSRAM total</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.controller.psram_total + " bytes</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">PSRAM free</span></td><td style=\"text-align:right;\"><div id=\"psram_free\"><span class=\"info_text_light\">" + status_psram_free + " bytes</span></div></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:20%;\"><span class=\"info_text\">MAC</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.wifi.mac + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">WiFi SSID</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.wifi.ssid + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:40%;\"><span class=\"info_text\">WiFi Rssi</span></td><td style=\"text-align:right;\"><div id=\"wifi_rssi\"><span class=\"info_text_light\">" + status_wifi_rssi + " dBm</span></div></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
    }

    // MQTT
    if (view_type=="mqtt") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">Active</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "    <select id='mqtt_active'>";
        if (!json_obj.mqtt.active) result_text += "<option value=\"0\" selected>No</option>";
        else result_text += "    <option value=\"0\">No</option>";
        if (json_obj.mqtt.active) result_text += "<option value=\"1\" selected>Yes</option>";
        else result_text += "    <option value=\"1\">Yes</option>"; 
        result_text += "    </select>";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:25%;\"><span class=\"info_text\">Status</span></td><td style=\"text-align:right;\"><div id=\"mqtt_status\"><span class=\"info_text_light\">" + status_mqtt + "</span></div></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Server</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"mqtt_server\" class=\"input_text_light\" size=\"15\" maxlength=\"15\" value=\"" + json_obj.mqtt.server + "\">";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Port</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"mqtt_port\" class=\"input_text_light\" size=\"5\" maxlength=\"5\" value=\"" + json_obj.mqtt.port + "\">";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">User</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"mqtt_user\" class=\"input_text_light\" size=\"15\" maxlength=\"15\" value=\"" + json_obj.mqtt.user + "\">";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Password</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <input type=\"text\" id=\"mqtt_password\" class=\"input_text_light\" size=\"15\" maxlength=\"15\" value=\"" + json_obj.mqtt.password + "\">";
        result_text += "    </span></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"saveMqtt()\">Save (restart)</span><br>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
    }

    // Pins
    if (view_type=="pins") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Board</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.board.name + "</span></td></tr></table>";       
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        // Pin Access Point
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">Pin Access Point</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "      <select id='board_pin_ap'>";
        result_text += "        <option value=\"" + json_obj.board.pin_ap + "\" selected>" + json_obj.board.pin_ap + "</option>";
        for (var i = 0; i < json_obj.pins.length; i++) {
            result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
        }     
        result_text += "      </select>";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        // Pin I2C sda
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">Pin I2C sda</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "      <select id='board_pin_i2c_sda'>";
        result_text += "        <option value=\"" + json_obj.board.pin_i2c_sda + "\" selected>" + json_obj.board.pin_i2c_sda + "</option>";
        for (var i = 0; i < json_obj.pins.length; i++) {
            result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
        }     
        result_text += "      </select>";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        // Pin I2C scl
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">Pin I2C scl</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
        result_text += "        <select id='board_pin_i2c_scl'>";
        result_text += "          <option value=\"" + json_obj.board.pin_i2c_scl + "\" selected>" + json_obj.board.pin_i2c_scl + "</option>";
        for (var i = 0; i < json_obj.pins.length; i++) {
            result_text += "      <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
        }     
        result_text += "        </select>";
        result_text += "    </span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin SPI MOSI</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.board.pin_spi_mosi + "</span></td></tr></table>";       
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin SPI CLK</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + json_obj.board.pin_spi_clk + "</span></td></tr></table>";       
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"saveBoard()\">Save (restart)</span><br>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
    }

    // WIFI
    if (view_type=="wifi") {
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">WIFI Readme</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text_light\">After clicking on \"Start Access Point\" the controller will disconnect the actual WiFi network and open its own access point with its own access data.<br>See GitHub for more information.</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text_light\">Connect to this WiFi network with your smartphone and enter the new WiFi access data for your home network.</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text_light\">After a certain time, the controller will automatically reconnect to the old network if nothing has been changed.</span></td></tr></table>";
        result_text += "</div></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <span class=\"button_text\" onclick=\"wifiController();\">Start Access Point</span><br>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <span class=\"button_text\" onclick=\"closeSettings();\">Close</span><br>";
        result_text += "</div></td></tr>";
    }

    result_text += "<tr><td style=\"height:5px;\"></td></tr>";
    result_text += "</tr></table>";
    result_text += "</div>";

    if (fade==true) document.getElementById('box_setting_frame').style.opacity = 0;
    document.getElementById('box_setting_frame').innerHTML = result_text;
    if (fade==true) FadeIn('box_setting_frame');
}

function openSettings(device_id,view_type = "device",fade = true, overrule_device_type = "") {
    grayOut();
    var device_new_allowed = true;
    var device_name = "New Device";device_homekit_id = "none";device_type = "led";device_pic = 0;
    var device_text_1 = "3C7EDBFFBDC37E3C";var device_text_2 = "3C7EDBFFC3BD7E3C";var device_text_3 = "221C3E2A3E1C2200";
    var device_pin_1 = 0;device_pin_2 = 0;device_pin_3 = 0;device_pin_4 = 0;
    var device_pin_1_reverse = 0;device_pin_2_reverse = 0;device_pin_3_reverse = 0;device_pin_4_reverse = 0;
    var device_bool_1 = 0;
    var device_float_1 = 0.0;device_float_2 = 0.0
    var device_restartrequired = 0;
    if (json_obj.homekit.activedevices >= json_obj.homekit.maximumdevices) device_new_allowed = false;
    for (var i = 0; i < json_obj.homekit_devices.length; i++) {
        if (json_obj.homekit_devices[i].id == device_id) {
            var obj = json_obj.homekit_devices[i];
            device_name = obj.name;device_homekit_id = obj.homekit_id;device_type = obj.type.code;device_pic = obj.type.picture_id;
            device_text_1 = obj.text_1;device_text_2 = obj.text_2;device_text_3 = obj.text_3;
            device_pin_1 = obj.pin_1;device_pin_2 = obj.pin_2;device_pin_3 = obj.pin_3;device_pin_4 = obj.pin_4;
            device_pin_1_reverse = obj.pin_1_reverse;device_pin_2_reverse = obj.pin_2_reverse;device_pin_3_reverse = obj.pin_3_reverse;device_pin_4_reverse = obj.pin_4_reverse;
            device_bool_1 = obj.bool_1;
            device_float_1 = obj.float_1;device_float_2 = obj.float_2;
            device_restartrequired = obj.restartrequired;
        }
    }
    if (overrule_device_type!="") device_type = overrule_device_type;
    var pic = "";
    for (var i = 0; i < json_obj_pic.length; i++){
        if (json_obj_pic[i].id==device_pic) pic = '<img width="30px height="30px" style="margin-top:5px;" src="data:' + json_obj_pic[i].data + ',' + json_obj_pic[i].base64 + '">';
    }
    var result_text = "";
    result_text += "<div id=\"box_settings\" class=\"box_settings\">";
    result_text += "";
    result_text += "<table width=\"100%\">";
    if (device_id==0) result_text += "<tr><td class=\"box_header\">New Device</td></tr>";
    else if (view_type=="device") result_text += "<tr><td class=\"box_header\"><span style=\"cursor:pointer;\" onclick=\"openSettings(" + device_id + ",'device',false)\">Device</span> | <span style=\"cursor:pointer;color:gray;\" onclick=\"openSettings(" + device_id + ",'wiring',false)\">Wiring</span></td></tr>";
    else result_text += "<tr><td class=\"box_header\"><span style=\"cursor:pointer;color:gray;\" onclick=\"openSettings(" + device_id + ",'device',false)\">Device</span> | <span style=\"cursor:pointer;\" onclick=\"openSettings(" + device_id + ",'wiring',false)\">Wiring</span></td></tr>";
    result_text += "<tr><td style=\"height:5px;\"></td></tr>";
    result_text += "<tr><td>";
    result_text += "    <div class=\"box_input\">";
    result_text += "        <table><tr><td><div id=\"div_device_logo\" class=\"div_device_logo\">" + pic + "</div></td>";
    result_text += "        <td width=\"10px\"></td><td><input type=\"text\" id=\"device_name\" class=\"input_text\" maxlength=\"15\" value=\"" + device_name + "\"></td></tr></table>";
    result_text += "    </div>";
    result_text += "</td></tr>";
    // CONTROLLER RESTART
    if (device_restartrequired) {
        result_text += "<tr><td><div class=\"box_input_red\">";
            result_text += "<table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:100%;\"><span class=\"info_text\">Controller restart required!</span></td></tr></table>";
        result_text += "</div></td></tr>";
    }
    // NO FREE DEVICES
    if (device_id==0 && device_new_allowed==false) {
        result_text += "<tr><td><div class=\"box_input_red\">";
            result_text += "<table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:100%;\"><span class=\"info_text\">Max. number of devices exceeded!</span></td></tr></table>";
        result_text += "</div></td></tr>";
    }
    // WIRING
    if (view_type=="wiring" && device_id!=0) {
        const arrWiring = [];
        if (device_type=="led") {
            arrWiring.push("ESP32 -> LED", "Board GND -> LED Kathode", "Board Pin -> Resistor -> LED Anode");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "          __   <br>";
            result_text += "         |__|     <br>";
            result_text += "         |__|     <br>";
            result_text += "          ||     <br>";
            result_text += "          ||<br>";
            result_text += "  Kathode || <br>";
            result_text += "           | Anode <br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="rgbled") {
            arrWiring.push("ESP32 -> RGB LED", "Board GND -> LED Kathode", "Board Pin -> Resistor -> LED Red Anode","Board Pin -> Resistor -> LED Green Anode","Board Pin -> Resistor -> LED Blue Anode");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "          ____   <br>";
            result_text += "         |____|     <br>";
            result_text += "         |____|     <br>";
            result_text += "          ||||     <br>";
            result_text += "          |||| Anode Blue<br>";
            result_text += "  Kathode ||| Anode Green <br>";
            result_text += "           | Anode Red <br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="contact") {
            arrWiring.push("ESP32 -> Potential free contact","Board 3v3 -> Contact pin 1","Board Pin GPIO -> Contact pin 2","Board Pin GPIO -> 10kOhm -> Board GND");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "       -------------- 3v3  <br>";
            result_text += "      |               <br>";
            result_text += "   \\        10kOhm   <br>";
            result_text += "    \\       --X----- GND<br>";
            result_text += "     \\     |          <br>";
            result_text += "      |     |          <br>";
            result_text += "       -------------- Pin  <br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="security" || device_type=="terxon") {
            arrWiring.push("ESP32 -> Potential free contact","Board 3v3 -> Contact pin 1","Board Pin GPIO -> Contact pin 2","Board Pin GPIO -> 10kOhm -> Board GND");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "       -------------- 3v3  <br>";
            result_text += "      |               <br>";
            result_text += "   \\        10kOhm   <br>";
            result_text += "    \\       --X----- GND<br>";
            result_text += "     \\     |          <br>";
            result_text += "      |     |          <br>";
            result_text += "       -------------- Pin  <br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="button" || device_type=="doorbell") {
            arrWiring.push("ESP32 -> Potential free button","Board GND -> Button pin 2","Board Pin GPIO -> Button pin 1","Board Pin GPIO -> 10kOhm -> Board 3v3");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "          | pin 2    <br>";
            result_text += "          |     <br>";
            result_text += "       \\   <br>";
            result_text += "     |--\\   <br>";
            result_text += "         \\   <br>";
            result_text += "          |   <br>";
            result_text += "          | pin 1  <br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="ds18b20") {
            arrWiring.push("ESP32 -> Temp Sensor DS18B20","Board 5V -> VCC (red)","Board GND -> GND (black)","Board Pin ADC -> DQ (yellow)","Board Pin ADC -> 4,7 kOhm -> 5v");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "         ------- black (GND)<br>";
            result_text += " ____   |     <br>";
            result_text += "(____|--|------- red (VCC)<br>";
            result_text += "        |    |<br>";
            result_text += "        |    X 4,7 kOhm<br>";
            result_text += "        |    |<br>";
            result_text += "         ------- yellow (DQ)<br>";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="dht11" || device_type=="dht22") {
            arrWiring.push("ESP32 -> Sensor DHT22","Board 3V3 -> VCC","Board 3V3 -> 10kOhm -> D","Board Pin ADC -> D","Board GND -> GND");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "       ________<br>";
            result_text += "      |        |<br>";
            result_text += "      |  DHT   |<br>";
            result_text += "      |________|<br>";
            result_text += "      |  |  |  |<br>";
            result_text += "    VCC  D  N  GND";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="bh1750") {
            arrWiring.push("ESP32 -> Sensor BH1750","3V3 -> VCC","GND -> GND","Pin " + json_obj.board.pin_i2c_scl + " (I2C SCL) -> SCL","Pin " + json_obj.board.pin_i2c_sda + " (I2C SDA) -> SDA");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "    _______________<br>";
            result_text += "   |               |<br>";
            result_text += "   |    BH1750     |<br>";
            result_text += "   |_______________|<br>";
            result_text += "   |   |   |   |   |<br>";
            result_text += "  VCC GND SCL SDA ADR";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="maxled") {
            arrWiring.push("ESP32 -> Sensor MAX7219","3V3 -> VCC","GND -> GND","Pin " + json_obj.board.pin_spi_mosi + " (SPI MOSI) -> MOSI/DIN","Pin " + json_obj.board.pin_spi_clk + " (SPI CLK) -> CLK/SCK","Pin " + device_pin_1 + " -> CS/SS");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "    _______________<br>";
            result_text += "   |               |<br>";
            result_text += "   |    MAX7219    |<br>";
            result_text += "   |_______________|<br>";
            result_text += "   |   |   |   |   |<br>";
            result_text += "  VCC GND MOSI CS CLK<br>";
            result_text += "          DIN  SS SCK";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="temt6000") {
            arrWiring.push("ESP32 -> Sensor TEMT6000","Board 3V3 -> VCC","Board GND -> GND","Board Pin ADC1 -> OUT");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "    _______________<br>";
            result_text += "   |               |<br>";
            result_text += "   |   TEMT6000    |<br>";
            result_text += "   |_______________|<br>";
            result_text += "   |      |       |  <br>";
            result_text += "  VCC    GND     OUT ";
            result_text += "</pre></div></td></tr>";
        }
        if (device_type=="sw420" || device_type=="hcsr501" || device_type=="mq2") {
            arrWiring.push("ESP32 -> Sensor SW420/HCSR501","Board 3V3 -> VCC","Board GND -> GND","Board Pin GPIO DI -> DO");
            result_text += "<tr><td><div class=\"box_input\"><pre>";
            result_text += "   ________     _________ <br>";
            result_text += "  | MQ-2   |   |         |<br>";
            result_text += "  | SW420  |   | HCSR501 |<br>";
            result_text += "  |________|   |_________|<br>";
            result_text += "  |   |    |   |    |    |<br>";
            result_text += " DO  GND  VCC  GND DO  VCC";
            result_text += "</pre></div></td></tr>";
        }
        result_text += "<tr><td><div class=\"box_input\">";
        for (var i = 0; i < arrWiring.length; i++){
            if (i>=1) result_text += "<hr style=\"border: 0.5px solid #f2f2f2;\">";
            if (i==0)result_text += "<table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">" + arrWiring[i] + "</span></td></tr></table>";
            if (i>=1) result_text += "<table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text_light\">" + arrWiring[i] + "</span></td></tr></table>";
        }
        result_text += "</div></td></tr>";
        result_text += "<tr><td style=\"height:5px;\"></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "<span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
    }

    // DEVICE
    if (view_type=="device") {
        // Pin name (GPIO=General Purpose Input/Output, ADC=AnalogDigitalConverter(In), DAC=DigitalAnalogConverter(Out))
        name_pin_1 = "";name_pin_2 = "";name_pin_3 = "";name_pin_4 = "";
        name_text_1 = "";name_text_2 = "";name_text_3 = "";
        name_bool_1 = "";
        name_float_1 = "";name_float_2 = "";
        if (device_type=="contact") {name_pin_1 = "Contact";};
        if (device_type=="security") {name_pin_1 = "Contact";name_pin_2 = "Armed";};
        if (device_type=="terxon") {name_pin_1 = "Away";name_pin_2 = "Triggered";name_pin_3 = "Night";name_pin_4 = "Stay";};
        if (device_type=="contact") {name_pin_1 = "Contact";};
        if (device_type=="led") {name_pin_1 = "";name_bool_1 = "Dimmable";};
        if (device_type=="rgbled") {name_pin_1 = "Red";name_pin_2 = "Green";name_pin_3 = "Blue";};
        if (device_type=="maxled") {name_pin_1 = "CS/SS";name_text_1 = "Sym1";name_text_2 = "Sym2";name_text_3 = "Sym3";};
        if (device_type=="temt6000") {name_pin_1 = "ADC IN";name_float_1 = "Light Offset";}
        if (device_type=="ds18b20") {name_pin_1 = "ADC IN";name_float_1 = "Temp. Offset";name_bool_1 = "Celsius";}
        if (device_type=="dht11" || device_type=="dht22") {name_pin_1 = "ADC IN";name_float_1 = "Temp. Offset";name_float_2 = "Humidity Offset";name_bool_1 = "Celsius";}
        if (device_type=="sw420" || device_type=="hcsr501" || device_type=="mq2") {name_pin_1 = "GPIO IN";name_float_1 = "Motion Timer (millsec)";}
        if (device_type=="bh1750") name_float_1 = "Light Offset";
        if (device_type=="outlet") name_pin_1 = "GPIO OUT";
        result_text += "<tr><td><div class=\"box_input\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Device ID</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + device_id + "</span></td></tr></table>";
        result_text += "    <hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">HomeKit ID</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">" + device_homekit_id + "</span></td></tr></table>";
        result_text += "</div></td></tr>";
        if (device_id!=0) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:20%;\"><span class=\"info_text\">Type</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_type' onchange=\"changeType(this)\" style=\"cursor: default;opacity:0.7;\" disabled>";
            result_text += "        <option value=\"" + device_type + "\" selected>" + device_type + "</option>";
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        else {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Type</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_type' onchange=\"changeType(this)\">";
            for (var i = 0; i < json_obj.types.length; i++) {
                if (json_obj.types[i].code==device_type) result_text += "    <option value=\"" + json_obj.types[i].code + "\" selected>" + json_obj.types[i].name + "</option>";
                else result_text += "    <option value=\"" + json_obj.types[i].code + "\">" + json_obj.types[i].name + "</option>";
            }   
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Pin 1
        if (['led','rgbled','maxled','ds18b20','dht11','dht22','security','button','contact','doorbell','terxon','leak','temt6000','sw420','hcsr501','mq2','outlet'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin " + name_pin_1 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";         
            result_text += "    <select id='device_pin_1'>";
            result_text += "        <option value=\"" + device_pin_1 + "\" selected>" + device_pin_1 + "</option>";
            for (var i = 0; i < json_obj.pins.length; i++) {
                if (json_obj.pins[i]==device_pin_1) result_text += "    <option value=\"" + json_obj.pins[i] + "\" selected>" + json_obj.pins[i] + "</option>";
                else result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
            }     
            result_text += "    </select>";
            result_text += "</span></td></tr></table>";
            // Pin 1 reverse
            if (['contact'].includes(device_type)) {
                result_text += "<hr style=\"border: 0.5px solid #f2f2f2;\">";
                result_text += "<table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Reverse Pin</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">"; 
                result_text += "    <select id='device_pin_1_reverse'>";
                if (device_pin_1_reverse) result_text += "<option value=\"1\" selected>Yes</option><option value=\"0\">No</option>";
                else result_text += "<option value=\"0\" selected>No</option><option value=\"1\">Yes</option>"; 
                result_text += "    </select>";   
                result_text += "</span></td></tr></table>";         
            }
            result_text += "</div></td></tr>";
        }
        // Pin 2
        if (['security','terxon','rgbled'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin " + name_pin_2 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_pin_2'>";
            result_text += "        <option value=\"" + device_pin_2 + "\" selected>" + device_pin_2 + "</option>";
            for (var i = 0; i < json_obj.pins.length; i++) {
                if (json_obj.pins[i]==device_pin_2) result_text += "    <option value=\"" + json_obj.pins[i] + "\" selected>" + json_obj.pins[i] + "</option>";
                else result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
            }     
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Pin 3
        if (['terxon','rgbled'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin " + name_pin_3 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_pin_3'>";
            result_text += "        <option value=\"" + device_pin_3 + "\" selected>" + device_pin_3 + "</option>";
            for (var i = 0; i < json_obj.pins.length; i++) {
                if (json_obj.pins[i]==device_pin_3) result_text += "    <option value=\"" + json_obj.pins[i] + "\" selected>" + json_obj.pins[i] + "</option>";
                else result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
            }     
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Pin 4
        if (['terxon'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:50%;\"><span class=\"info_text\">Pin " + name_pin_4 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_pin_4'>";
            result_text += "        <option value=\"" + device_pin_4 + "\" selected>" + device_pin_4 + "</option>";
            for (var i = 0; i < json_obj.pins.length; i++) {
                if (json_obj.pins[i]==device_pin_4) result_text += "    <option value=\"" + json_obj.pins[i] + "\" selected>" + json_obj.pins[i] + "</option>";
                else result_text += "    <option value=\"" + json_obj.pins[i] + "\">" + json_obj.pins[i] + "</option>";
            }     
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Text 1
        if (['maxled'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:30%;\"><span class=\"info_text\">" + name_text_1 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <input type=\"text\" id=\"device_text_1\" class=\"input_text_light\" size=\"16\" maxlength=\"16\" value=\"" + device_text_1 + "\">";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Text 2
        if (['maxled'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:30%;\"><span class=\"info_text\">" + name_text_2 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <input type=\"text\" id=\"device_text_2\" class=\"input_text_light\" size=\"16\" maxlength=\"16\" value=\"" + device_text_2 + "\">";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Text 3
        if (['maxled'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:30%;\"><span class=\"info_text\">" + name_text_3 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <input type=\"text\" id=\"device_text_3\" class=\"input_text_light\" size=\"16\" maxlength=\"16\" value=\"" + device_text_3 + "\">";
            result_text += "</span></td></tr></table></div></td></tr>";
        }
        // Bool 1
        if (['led','dht11','dht22','ds18b20'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\"><table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:60%;\"><span class=\"info_text\">" + name_bool_1 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "    <select id='device_bool_1'>";
            if (device_bool_1==0) result_text += "<option value=\"0\" selected>No</option>";
            else result_text += "    <option value=\"0\">No</option>";
            if (device_bool_1==1) result_text += "<option value=\"1\" selected>Yes</option>";
            else result_text += "    <option value=\"1\">Yes</option>"; 
            result_text += "    </select>";
            result_text += "</span></td></tr></table></div></td></tr>";
        } 
        // Float 1
        if (['dht11','dht22','ds18b20','bh1750','temt6000','sw420','hcsr501','mq2'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\">";
            result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:70%;\"><span class=\"info_text\">" + name_float_1 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "        <input type=\"text\" id=\"device_float_1\" class=\"input_text_light\" size=\"5\" maxlength=\"5\" value=\"" + device_float_1 + "\" onkeypress=\"return charFloat(event.charCode)\">";
            result_text += "        </span></td></tr></table>";
            result_text += "</div></td></tr>";
        }
        // Float 2
        if (['dht11','dht22'].includes(device_type)) {
            result_text += "<tr><td><div class=\"box_input\">";
            result_text += "    <table style=\"width:100%;border:0px;padding:0;margin:0;\"><tr><td style=\"width:70%;\"><span class=\"info_text\">" + name_float_2 + "</span></td><td style=\"text-align:right;\"><span class=\"info_text_light\">";
            result_text += "        <input type=\"text\" id=\"device_float_2\" class=\"input_text_light\" size=\"5\" maxlength=\"5\" value=\"" + device_float_2 + "\" onkeypress=\"return charFloat(event.charCode)\">";
            result_text += "        </span></td></tr></table>";
            result_text += "</div></td></tr>";
        }

        result_text += "<tr><td style=\"height:5px;\"></td></tr>";
        result_text += "<tr><td><div class=\"box_input\">";
        if (device_id!=0 || device_new_allowed==true) result_text += "<span class=\"button_text\" onclick=\"saveDevice(" + device_id + ")\">Save Device</span><br>";
        if (device_id!=0 || device_new_allowed==true) result_text += "<hr style=\"border: 0.5px solid #f2f2f2;\">";
        if (device_id!=0) result_text += "<span class=\"button_text\" onclick=\"deleteDevice(" + device_id + ")\">Delete Device</span><br>";
        if (device_id!=0) result_text += "<hr style=\"border: 0.5px solid #f2f2f2;\">";
        result_text += "<span class=\"button_text\" onclick=\"closeSettings()\">Close</span><br>";
        result_text += "</div></td></tr>";
    }
    result_text += "<tr><td style=\"height:5px;\"></td></tr>";
    result_text += "</table>";

    if (fade==true) document.getElementById('box_setting_frame').style.opacity = 0;
    document.getElementById('box_setting_frame').innerHTML = result_text;
    if (fade==true) FadeIn('box_setting_frame');
}
)=====";