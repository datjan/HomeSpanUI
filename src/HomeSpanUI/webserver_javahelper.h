const char index_javahelper[] PROGMEM = R"=====(
function closeSettings() {
    grayIn();
    FadeOut('box_setting_frame');
}
function saveDevice(device_id) {
    if(xmlHttpDeviceSave.readyState==0 || xmlHttpDeviceSave.readyState==4){
        var params = "device_id=" + device_id;
        params += "&device_name=" + document.getElementById('device_name').value;
        params += "&device_type=" + document.getElementById('device_type').value;        
        if (['led','rgbled','maxled','ds18b20','dht11','dht22','security','button','contact','doorbell','terxon','leak','temt6000','sw420','hcsr501','mq2','outlet'].includes(device_type)) {
            params += "&device_pin_1=" + document.getElementById('device_pin_1').value;
        }
        if (['contact'].includes(device_type)) {
            params += "&device_pin_1_reverse=" + document.getElementById('device_pin_1_reverse').value;
        }
        if (['security','terxon','rgbled'].includes(device_type)) {
            params += "&device_pin_2=" + document.getElementById('device_pin_2').value;
        }
        if (['terxon','rgbled'].includes(device_type)) {
            params += "&device_pin_3=" + document.getElementById('device_pin_3').value;
        }
        if (['terxon'].includes(device_type)) {
            params += "&device_pin_4=" + document.getElementById('device_pin_4').value;
        }
        if (['maxled'].includes(device_type)) {
            params += "&device_text_1=" + document.getElementById('device_text_1').value;
        }
        if (['maxled'].includes(device_type)) {
            params += "&device_text_2=" + document.getElementById('device_text_2').value;
        }
        if (['maxled'].includes(device_type)) {
            params += "&device_text_3=" + document.getElementById('device_text_3').value;
        }
        if (['led'].includes(device_type)) {
            params += "&device_bool_1=" + document.getElementById('device_bool_1').value;
        }
        if (['dht11','dht22','ds18b20','bh1750','temt6000','sw420','hcsr501','mq2'].includes(device_type)) {
            params += "&device_float_1=" + document.getElementById('device_float_1').value;
        }
        if (['dht11','dht22'].includes(device_type)) {
            params += "&device_float_2=" + document.getElementById('device_float_2').value;
        }
        xmlHttpDeviceSave.open('POST','devicesave'+methodending,true);
        xmlHttpDeviceSave.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xmlHttpDeviceSave.onreadystatechange=function() { 
            if(xmlHttpDeviceSave.readyState==4 && xmlHttpDeviceSave.status==200){
                checkJsonShowError(xmlHttpDeviceSave.response);
                actControllerAndDevices();  
                showRestart();
            }
        }
        xmlHttpDeviceSave.send(params);
        FadeOut('box_setting_frame');
        showWaiting("device_add");
    }
}
function deleteDevice(device_id) {
    if(xmlHttpDeviceDelete.readyState==0 || xmlHttpDeviceDelete.readyState==4){
        var params = 'device_id=' + device_id;
        xmlHttpDeviceDelete.open('POST','devicedelete'+methodending,true);
        xmlHttpDeviceDelete.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xmlHttpDeviceDelete.onreadystatechange=function() { 
            if(xmlHttpDeviceDelete.readyState==4 && xmlHttpDeviceDelete.status==200){
                checkJsonShowError(xmlHttpDeviceDelete.response);
                actControllerAndDevices();  
                showRestart();
            }
        }
        xmlHttpDeviceDelete.send(params);
        FadeOut('box_setting_frame');
        showWaiting("device_delete");
    }
}
// Board
function saveBoard() {
    if(xmlHttpBoardSave.readyState==0 || xmlHttpBoardSave.readyState==4){
        var params = "board=1";
        if (document.getElementById('board_pin_ap')) params += "&board_pin_ap=" + document.getElementById('board_pin_ap').value;
        if (document.getElementById('board_pin_i2c_sda')) params += "&board_pin_i2c_sda=" + document.getElementById('board_pin_i2c_sda').value;
        if (document.getElementById('board_pin_i2c_scl')) params += "&board_pin_i2c_scl=" + document.getElementById('board_pin_i2c_scl').value;
        xmlHttpBoardSave.open('POST','boardsave'+methodending,true);
        xmlHttpBoardSave.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xmlHttpBoardSave.onreadystatechange=function() { 
            if(xmlHttpBoardSave.readyState==4 && xmlHttpBoardSave.status==200){
                checkJsonShowError(xmlHttpBoardSave.response);
            }
        }
        xmlHttpBoardSave.send(params);
        FadeOut('box_setting_frame');
        // Wait for restart
        showWaitingPeriodAndReloadSite(12);
    }
}
// Controller
function saveController() {
    if(xmlHttpControllerSave.readyState==0 || xmlHttpControllerSave.readyState==4){
        var params = "controller=1";
        if (document.getElementById('controller_homekit_name')) params += "&controller_homekit_name=" + document.getElementById('controller_homekit_name').value;
        if (document.getElementById('controller_homekit_type')) params += "&controller_homekit_type=" + document.getElementById('controller_homekit_type').value;
        if (document.getElementById('controller_homekit_code')) params += "&controller_homekit_code=" + document.getElementById('controller_homekit_code').value;
        if (document.getElementById('controller_homekit_port')) params += "&controller_homekit_port=" + document.getElementById('controller_homekit_port').value;
        if (document.getElementById('controller_board_setup_manual')) params += "&controller_board_setup_manual=" + document.getElementById('controller_board_setup_manual').value;
        if (document.getElementById('controller_board_setup')) params += "&controller_board_setup=" + document.getElementById('controller_board_setup').value;
        xmlHttpControllerSave.open('POST','controllersave'+methodending,true);
        xmlHttpControllerSave.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xmlHttpControllerSave.onreadystatechange=function() { 
            if(xmlHttpControllerSave.readyState==4 && xmlHttpControllerSave.status==200){checkJsonShowError(xmlHttpControllerSave.response);}
        }
        xmlHttpControllerSave.send(params);
        FadeOut('box_setting_frame');
        // Wait for restart
        showWaitingPeriodAndReloadSite(12);
    }
}
function unpairController() {
    // Request reboot
    if(xmlHttpControllerRestart.readyState==0 || xmlHttpControllerRestart.readyState==4){
        xmlHttpControllerRestart.open('GET','unpair'+methodending,true);
        xmlHttpControllerRestart.send(null);
    }
    closeSettings();
    // Wait for restart
    showWaitingPeriodAndReloadSite(12);
}
function restartController() {
    // Request reboot
    if(xmlHttpControllerRestart.readyState==0 || xmlHttpControllerRestart.readyState==4){
        xmlHttpControllerRestart.open('GET','reboot'+methodending,true);
        xmlHttpControllerRestart.send(null);
    }
    closeSettings();
    // Wait for restart
    showWaitingPeriodAndReloadSite(12);
}
function resetController() {
    // Request reboot
    if(xmlHttpControllerRestart.readyState==0 || xmlHttpControllerRestart.readyState==4){
        xmlHttpControllerRestart.open('GET','reset'+methodending,true);
        xmlHttpControllerRestart.send(null);
    }
    closeSettings();
    // Wait for restart
    showWaitingPeriodAndReloadSite(12);
}
function wifiController() {
    // Request wifi ap
    if(xmlHttpControllerRestart.readyState==0 || xmlHttpControllerRestart.readyState==4){
        xmlHttpControllerRestart.open('GET','ap'+methodending,true);
        xmlHttpControllerRestart.send(null);
    }
    closeSettings();
    // Wait for restart
    //showWaitingPeriodAndReloadSite(12);
}     
// Loading
function showWaiting(type = "") {
    // Gray Out
    grayOut();
    // Loading text
    if (type=="pictures") document.getElementById('loader_message').innerHTML = "loading<br>pictures";
    else if (type=="devices") document.getElementById('loader_message').innerHTML = "loading<br>devices";
    else if (type=="device_add") document.getElementById('loader_message').innerHTML = "add<br>device";
    else if (type=="device_delete") document.getElementById('loader_message').innerHTML = "delete<br>device";
    else if (type=="controller_restart") document.getElementById('loader_message').innerHTML = "controller<br>restart";
    else document.getElementById('loader_message').innerHTML = "";
    // Loading
    FadeIn('loader_message');
    FadeIn('loader');
}
function hideWaiting() {
    // Gray Out In
    grayIn();
    // Loading
    FadeOut('loader_message');
    FadeOut('loader');
}
const showWaitingPeriodAndReloadSite = async (wait_seconds) => {
    // Loading
    showWaiting(type = "controller_restart");
    // Waiting
    while (wait_seconds > 0) { 
        if (abort_reload_site == true) break;
        document.getElementById('loader_message').innerHTML = "restart<br>" + wait_seconds + " sec";
        wait_seconds = wait_seconds - 1;
        await delay(1000);
    }
    // Stop Loading
    hideWaiting();
    // Reload Site
    reloadSite();
}
function reloadSite() {
    if (abort_reload_site == false) window.location.reload();
    abort_reload_site = false;
}
// Error
function checkJsonShowError(json_to_check, restart = false) {
    try {
        json_obj_temp = JSON.parse(json_to_check);
    } catch (e) {
        hideWaiting();
        showError("Controller JSON response invalid",restart);
        return false;
    }
    if(json_obj_temp.success==0) showError(json_obj_temp.message,restart);
    if(json_obj_temp.success==0) abort_reload_site = true;
}
function showError(message = "", restart = false) {
    // Error text
    document.getElementById('box_message').innerHTML = "<a onclick=\"hideError();\">" + message + "</a> ";
    if (restart) document.getElementById('box_message').innerHTML += " | <a class=\"reset_text\" onclick=\"resetController();\"> reset?</a>";
    // Show
    FadeIn('box_message');
}
function hideError() {
    FadeOut('box_message');
}
// Restart Message
function showRestart() {
    var result_text = "";
    result_text += "<div id=\"div_device_restart\" class=\"div_restart waiting_gray\" onclick=\"restartController();\">";
    result_text += "<span class=\"span_restart_text\">Controller Restart</span>";
    result_text += "</div>";
    document.getElementById('table_restart').style.opacity = 0;
    document.getElementById('restart_item').innerHTML = result_text;
    FadeIn('table_restart');
}
// Type Change Box
function changeType(selectObject) {
    var type = selectObject.value;  
    openSettings(0,"device",false,type);
}
// Gray Out
function grayOut() {
    // Gray Out
    const collection = document.getElementsByClassName("waiting_gray");
    for (let i = 0; i < collection.length; i++) {
        collection[i].classList.add("grayout");
    }
}
function grayIn() {
    // Gray Out In
    const collection = document.getElementsByClassName("waiting_gray");
    for (let i = 0; i < collection.length; i++) {
        collection[i].classList.remove("grayout");
    }
}
// Fade
const delay = ms => new Promise(res => setTimeout(res, ms));
const FadeIn = async (element_id) => {
    var ocpacity = 0;
    while (ocpacity < 1) { // Fade In
        ocpacity = ocpacity + 0.1;
        document.getElementById(element_id).style.opacity = ocpacity;
        await delay(20);
    }
}
const FadeOut = async (element_id) => {
    var ocpacity = 1;
    while (ocpacity > 0) { // Fade Out
        ocpacity = ocpacity - 0.1;
        document.getElementById(element_id).style.opacity = ocpacity;
        await delay(20);
    }
    document.getElementById(element_id).innerHTML = "";
}
const FadeOutSetContentFadeIn = async (element_id,content) => {
    var ocpacity = 1;
    while (ocpacity > 0) { // Fade Out
        ocpacity = ocpacity - 0.1;
        document.getElementById(element_id).style.opacity = ocpacity;
        await delay(20);
    }
    document.getElementById(element_id).innerHTML = content;
    var ocpacity = 0;
    while (ocpacity < 1) { // Fade In
        ocpacity = ocpacity + 0.1;
        document.getElementById(element_id).style.opacity = ocpacity;
        await delay(20);
    }
}
const FadeOutFadeIn = async (element_id) => {
    var ocpacity = 1;
    while (ocpacity > 0) { // Fade Out
        ocpacity = ocpacity - 0.1;
        document.getElementById(element_id).style.background=rgba(149, 158, 149, 0.6);
        await delay(20);
    }
    var ocpacity = 0;
    while (ocpacity < 1) { // Fade In
        ocpacity = ocpacity + 0.1;
        document.getElementById(element_id).style.opacity = ocpacity;
        await delay(20);
    }
}
// Char Filter
function charInt(char) {
    return (char >= 48 && char <= 57);
}
function charFloat(char) {
    return (char >= 48 && char <= 57) || char == 45 || char == 46;
}   
// QR Code
function generateQrCode(qrtext) {
    var qrcode = new QRCode(document.getElementById("qrcode"), {
        text: qrtext,
        width: 64,
        height: 64,
        colorDark : "#000000",
        colorLight : "#f2f2f2",
        correctLevel : QRCode.CorrectLevel.H
    });
    //qrcode.clear(); // clear the code.
}
)=====";
