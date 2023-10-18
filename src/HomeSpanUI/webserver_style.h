const char index_style[] PROGMEM = R"=====(
html{min-height: 100%;}
body {
    background-image:linear-gradient(42deg, #a5b8a3, #d4f5d0, #a5b8a3);
    background-repeat: no-repeat, no-repeat;
    background-size:cover;
}
table{border-collapse: collapse;}     
table, th, td {border: 0px solid blue;}
.hub_header
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 32px;
    font-weight: bold;
    color: #a5a5a5;
    background-color: transparent;
}
.div_hub_status
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: white;
    float: both;
    width:580px;
    margin-left:15px;
    margin-right:15px;
    margin-top:0px;
    margin-bottom:10px;
    padding-left:10px;
    padding-right:10px;
    padding-top:10px;
    padding-bottom:10px;
    border-top-right-radius: 1em;
    border-bottom-right-radius: 1em;
    border-top-left-radius: 1em;
    border-bottom-left-radius: 1em;
    border:0px solid #a6a6a6;
    background: rgba(149, 158, 149, 0.6);
    overflow:hidden;
}
.devices_header 
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 24px;
    font-weight: bold;
    color: #a5a5a5;
    background-color: transparent;
}
.div_device
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: white;
    float: left;
    width:275px;
    height:40px; 
    margin-left:5px;
    margin-right:5px;
    margin-top:5px;
    margin-bottom:5px;
    padding-left:10px;
    padding-right:5px;
    padding-top:5px;
    padding-bottom:5px;
    border-top-right-radius: 1em;
    border-bottom-right-radius: 1em;
    border-top-left-radius: 1em;
    border-bottom-left-radius: 1em;
    border:0px solid #a6a6a6;
    text-align:left;
    
    background: rgba(149, 158, 149, 0.6);
    overflow:hidden;
    cursor: pointer;
}
.div_device_marked
{
    background: rgba(255, 255, 255, 0.6) !important;
}
.div_device_logo
{
    line-height: 1.3;
    font-family: monospace;
    white-space: pre;
    color: yellow;
    text-align:center;
    vertical-align: middle;
    display: inline-block;
    width:40px;
    height:40px; 
    margin-left:0px;
    margin-right:0px;
    margin-top:0px;
    margin-bottom:0px;
    padding-left:0px;
    padding-right:0px;
    padding-top:0px;
    padding-bottom:0px;
    border-top-right-radius: 20px;
    border-bottom-right-radius: 20px;
    border-top-left-radius: 20px;
    border-bottom-left-radius: 20px;
    border:0px solid #a6a6a6;
    /*background-color:#788078;*/
    overflow:hidden;
    cursor: pointer;
}
.div_device_logo_switch
{
    line-height: 1.3;
    font-family: monospace;
    white-space: pre;
    color: yellow;
    text-align:center;
    vertical-align: middle;
    display: inline-block;
    width:40px;
    height:40px; 
    margin-left:0px;
    margin-right:0px;
    margin-top:0px;
    margin-bottom:0px;
    padding-left:0px;
    padding-right:0px;
    padding-top:0px;
    padding-bottom:0px;
    border-top-right-radius: 20px;
    border-bottom-right-radius: 20px;
    border-top-left-radius: 20px;
    border-bottom-left-radius: 20px;
    border:0px solid #a6a6a6;
    background-color:#788078;
    overflow:hidden;
    cursor: pointer;
}
.div_device_logo_switch_marked
{
    background: rgba(30, 170, 201, 1) !important;
}
.div_device_nobopama {border:0px;padding:0;margin:0;}
.span_device_name{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: white;
}
.span_device_name_marked{
    color: #383838 !important;
}
.span_device_comment{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 12px;
    font-weight: normal;
    color: #e6e6e6;
}
.span_device_comment_marked{
    color: #a1a1a1 !important;
}
.span_device_comment_red{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 12px;
    font-weight: bold;
    color: #ff6666;
}
.div_device_add
{
    width:26px;
    height:26px;
    border-radius:13px;
    font-family: Arial, Helvetica, sans-serif;
    font-size:18px;
    font-weight: bold;
    color:#fff;
    line-height:27px;
    text-align:center;
    background:#a5a5a5;
    cursor: pointer;
    float: left;
}
.div_device_reload
{
    width:26px;
    height:26px;
    border-radius:13px;
    font-family: Arial, Helvetica, sans-serif;
    font-size:20px;
    font-weight: normal;
    color:#fff;
    line-height:24px;
    text-align:center;
    background:#a5a5a5;
    cursor: pointer;
    float: left;
}

.box_message{
    left: 0;
    top: 0;
    width: 100%;
    height: 27px;
    position: fixed;
    z-index: 99;
    font-family: Arial, Helvetica, sans-serif;
    font-size:14px;
    font-weight: bold;
    color:black;
    text-align:center;
    background:#ec9999;
    padding-top:10px;
    opacity: 0;
    cursor: pointer;
  }
.box_settings {
    /*height: 250px; /* Höhe der div-Box */
    width: 300px; /* Breite der div-Box */
    margin-top: 0px; /* Damit der "Ausrichtungspunkt" in der Mitte der Box liegt */
    margin-left: -150px;
    position: absolute; /* positionieren */
    top: 10%; /* in die Mitte verschieben */
    left: 50%;
    z-index: 100;

    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: gray;
    border-top-right-radius: 20px;
    border-bottom-right-radius: 20px;
    border-top-left-radius: 20px;
    border-bottom-left-radius: 20px;
    border:0px solid #a6a6a6;
    background-color:#f2f2f2;
    overflow:hidden;
}
.box_footer {
    position:absolute;
    bottom: 0;
    width: 100%;
    height: 22px;
    left: 0%;

    padding-top:5px;
    
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    color: #d9f3e4;
    border:0px solid #a6a6a6;
    background-color:#8dcaa7;
    overflow:hidden;
    text-align:center;
}
.box_footer_link
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    color: #d9f3e4;
    background-color: transparent;
}
.box_header {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 20px;
    font-weight: bold;
    color: white;
    text-align:center;
    padding-left:5px;
    padding-right:5px;
    padding-top:5px;
    padding-bottom:5px;
    background: rgba(149, 158, 149, 0.6);
    cursor: default;
}
.box_input{
    float: left;
    width:244px;
    /*height:50px; */
    margin-left:13px;
    margin-right:13px;
    margin-top:5px;
    margin-bottom:5px;
    padding-left:14px;
    padding-right:14px;
    padding-top:7px;
    padding-bottom:7px;
    border-top-right-radius: 1em;
    border-bottom-right-radius: 1em;
    border-top-left-radius: 1em;
    border-bottom-left-radius: 1em;
    border:0px solid #a6a6a6;
    background: white;
    vertical-align: middle;
    overflow:hidden;
}
.box_input_red{
    float: left;
    width:244px;
    /*height:50px; */
    margin-left:13px;
    margin-right:13px;
    margin-top:5px;
    margin-bottom:5px;
    padding-left:14px;
    padding-right:14px;
    padding-top:7px;
    padding-bottom:7px;
    border-top-right-radius: 1em;
    border-bottom-right-radius: 1em;
    border-top-left-radius: 1em;
    border-bottom-left-radius: 1em;
    border:0px solid #a6a6a6;
    background: #ffcccc;
    vertical-align: middle;
    overflow:hidden;
}
.text_homekit_code {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 20px;
    font-weight: bold;
    color: #8dcaa7;
    text-align:center;
}
.input_text {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: gray;
    width: 100%; min-width: 100%; 
    box-sizing: border-box;
    padding: 4px;
    outline: none;
    border: 0;
    background-color: white;
    cursor: pointer;
}
.input_text_light {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    color: gray;
    width: 100%; min-width: 100%; 
    box-sizing: border-box;
    padding: 4px;
    outline: none;
    border: 0;
    background-color: white;
    text-align:right;
    cursor: pointer;
}
.info_text {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: gray;
    padding: 4px;
    outline: none;
    border: 0;
    cursor: default;
}
.info_text_light {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    color: gray;
    padding: 4px;
    outline: none;
    border: 0;
    cursor: default;
}
.info_text_italic {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    font-style: italic;
    color: gray;
    padding: 4px;
    outline: none;
    border: 0;
    cursor: default;
}
.button_text {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: orange;
    padding: 4px;
    outline: none;
    border: 0;
    background-color: white;
    cursor: pointer;
}
select {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: normal;
    color: gray;
    /*width: 50px; */
    font-size: 1.1em; 
    font-family: inherit;
    font-weight: 300;
    text-align: right;
    background-color: white;
    border: 0;
    cursor: pointer;
}
.div_restart
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: white;
    float: left;
    width:580px;
    height:30px; 
    margin-left:5px;
    margin-right:5px;
    margin-top:5px;
    margin-bottom:5px;
    padding-left:5px;
    padding-right:5px;
    padding-top:15px;
    padding-bottom:5px;
    border-top-right-radius: 1em;
    border-bottom-right-radius: 1em;
    border-top-left-radius: 1em;
    border-bottom-left-radius: 1em;
    border:0px solid #a6a6a6;
    text-align:center;
    background: rgba(255, 204, 204, 0.6);
    overflow:hidden;
    cursor: pointer;
}
.span_restart_text{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    text-decoration: none;
    color: red;
}

.loader_text
{
    font-family: Arial, Helvetica, sans-serif;
    font-size: 14px;
    font-weight: bold;
    color: #a5a5a5;
    float: both;
}
.loader {
    border: 4px solid #f3f3f3;
    border-radius: 50%;
    border-top: 4px solid #3498db;
    width: 20px;
    height: 20px;
    -webkit-animation: spin 2s linear infinite; /* Safari */
    animation: spin 2s linear infinite;
    }

    /* Safari */
    @-webkit-keyframes spin {
    0% { -webkit-transform: rotate(0deg); }
    100% { -webkit-transform: rotate(360deg); }
    }

    @keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
    }

@media (max-width: 650px)
{
    .hub_header { font-size: 20px !important; }
    .div_hub_status { width: 290px !important; }
    .devices_header { font-size: 20px !important; }
    .table_main { width: 300px !important; }
    .table_main_device { width: 135px !important; }
    .table_main_loader { width: 40px !important; }
    .table_main_loadertext { width: 65px !important; }
    .table_main_add { width: 30px !important; }
    .table_main_reload { width: 30px !important; }
    .div_restart { width: 280px !important; }
    .div_device { width: 280px !important; }
}
.grayout {
    filter: blur(1px) grayscale(50%);
    opacity: 0.6;
    pointer-events: none;
}
)=====";
