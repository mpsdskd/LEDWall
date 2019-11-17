void sendRoot() {
  Serial.println("Send Root");
  File page = SPIFFS.open("/root.html", "r");
  webserver.streamFile(page, "text/html");
  page.close();
}
String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = webserver.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload() { // upload a new file to the SPIFFS
  HTTPUpload& upload = webserver.upload();
  String path;
  if (upload.status == UPLOAD_FILE_START) {
    path = upload.filename;
    if (!path.startsWith("/")) path = "/" + path;
    if (!path.endsWith(".gz")) {                         // The file server always prefers a compressed version of a file
      String pathWithGz = path + ".gz";                  // So if an uploaded file is not compressed, the existing compressed
      if (SPIFFS.exists(pathWithGz))                     // version of that file must be deleted (if it exists)
        SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      //      webserver.sendHeader("Location", "/success.html");     // Redirect the client to the success page
      //      webserver.send(303);
      webserver.send(303, "text/plain", "Great success");
    } else {
      webserver.send(500, "text/plain", "500: couldn't create file");
    }
  }
  readStatus();
}
void handleNotFound() { // if the requested file or page doesn't exist, return a 404 not found error
  if (!handleFileRead(webserver.uri())) {        // check if the file exists in the flash memory (SPIFFS), if so, send it
    webserver.send(404, "text/plain", "404: File Not Found");
  }
}
static const String uploadForm = "<form method=\"post\" enctype=\"multipart/form-data\">"
                                 "<input type=\"file\" name=\"name\">"
                                 "<input class=\"button\" type=\"submit\" value=\"Upload\">"
                                 "</form>";

String createSunrisePage() {
  static const String page = "<head><title>LED Wall</title><style>html{ font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                             ".button{ background-color: #195B6A; border: none; color: white; padding: 4px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;width: 400px}"
                             ".button2 {background-color: #0000AA;}</style></head>"
                             "<h3>LED Wall - Settings</h3>"
                             "<form action=\"settings\" method=\"POST\">\n"
                             "<p>Alarm Time: <input id=\"time\" type=\"time\" name=\"time\" value=\"";
  String hourString = String((int)sunriseMinuteOfDay / 60);
  String minuteString = String((int)sunriseMinuteOfDay % 60);
  if (hourString.length() == 1) hourString = "0" + hourString;
  if (minuteString.length() == 1) minuteString = "0" + minuteString;
  String vp = "";
  vp += hourString;
  vp += String(":");
  vp += minuteString;
  vp += String( "\"></p>\n");
  vp += "<p>Sunrise Duration: <input type=\"number\" id=\"sunriseduration\" name=\"sunriseduration\" min=\"1\" max=\"30\" value=\"";
  vp += sunriseDuration;
  vp += "\"></p>";
  vp += "<p>Sunrise Brightness: <input type=\"number\" id=\"sunrisebrightness\" name=\"sunrisebrightness\" min=\"1\" max=\"255\" value=\"";
  vp += sunriseBrightness;
  vp += "\"></p>";
  vp += "<p>Enable Sunrise: <input type=\"checkbox\" id=\"enablesunrise\" name=\"enablesunrise\" value=\"enablesunrise\"";
  if (sunrise) vp += "checked";
  vp += "></p>";
  vp += "<p>Sensor Brightness: <input type=\"number\" id=\"sensorbrightness\" name=\"sensorbrightness\" min=\"1\" max=\"255\" value=\"";
  vp += sensorBrightness;
  vp += "\"></p>";
  vp += "<p>Sensor Duration: <input type=\"number\" id=\"sensorduration\" name=\"sensorduration\" min=\"1\" max=\"600\" value=\"";
  vp += sensorDuration;
  vp += "\"></p>";
  vp += "<p>Enable Sensor: <input type=\"checkbox\" id=\"enablesensor\" name=\"enablesensor\" value=\"enablesensor\"";
  if (sensorEnabled) vp += " checked";
  vp += "></p>"
        "<input name=\"Save\" class=\"button\" type=\"submit\" value=\"Save Settings\">"
        "</form>"
        "<p><a href=\".\"><button class=\"button button2\">Back</button></a></p>";

  return page + vp;
}

void sendSunrisePage() {
  webserver.send(200, "text/html  ", createSunrisePage());
}

String matrixInput() {
  String page = R"(<!--Thanks Rayshobby Shop https://rayshobby.net/wordpress/
https://rayshobby.net/wordpress/wifi-color-led-matrix/ -->
<head>
  <title>LEDWall Pixel Input</title>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    .color-sel { width: 24px; height: 24px; text-align: center;}
    .pixel-sel { width: 48px; height: 48px; background-color: black; }
    .button{ background-color: #195B6A; border: none; color: white; padding: 4px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;width: 400px}
    .button2 {background-color: #0000AA;}
    button { font-size: 16px; }
    p { font-size:18px; font-style:normal; line-height:0px; }
    table { border-color:lightgray; border-collapse:collapse; }
  </style>
</head>
<body>
  <script>
    var x=)";

  page += String(kMatrixWidth);

  page +=
    R"(;
    var y=)";

  page += String(kMatrixHeight);

  page +=
    R"(;
    var px=x*y;
    var nh=12,i,j,cc='rgb(255, 255, 255)';
    var cc_elem;
    function w(s) { document.write(s); }
    function id(s){ return document.getElementById(s); }
    function mark(e) {e.innerHTML='&#10004;';}
    function cs_click(e) {
      var bg=e.target.style.background;
      cc_elem.innerHTML='';
      mark(e.target);
      cc=bg;
      cc_elem=e.target;
    }
    w('<table border=1>');
    idx=0;
    for(i=0;i<2;i++) {
      w('<tr>');
      for(j=0;j<nh;j++,idx++) {
        w('<td class=color-sel id=cs'+idx+'></td>');
        var obj=id('cs'+idx);
        if(i==0) { var hue=(j/nh)*360; obj.style.background='hsl('+hue+',100%,50%)'; }
        else { var lit=(j/(nh-1))*100; obj.style.background='hsl(0,0%,'+lit+'%)'; }
        obj.addEventListener('click',cs_click);
      }
      w('</tr>');
    }
    w('</table>');
    cc_elem=id('cs'+(2*nh-1));
    mark(cc_elem);
  </script>
  <hr />
  <p>Set pixel color: </p>
  <p><button id='btn_fill'>Fill</button> <button id='btn_clear'>Clear</button> <button id='btn_submit'><b>Submit</b></button></p>
  <script>
    function ps_click(e) { if(e.target.style.background==cc) e.target.style.background='rgb(0,0,0)'; else e.target.style.background=cc; }
    w('<table border=1 style="border-collapse: separate; border-spacing: 2px;">');
    idx=0;
    for(i=0;i<y;i++) {
      w('<tr>');
      for(j=0;j<x;j++,idx++) {
        w('<td class=pixel-sel id=ps'+idx+'></td>');
        id('ps'+idx).addEventListener('click',ps_click);
        id('ps'+idx).style.background = 'rgb(0, 0, 0)';
      }
      w('</tr>');
    }
    w('</table>');
  </script>
  <script>
    function rgb2hex(orig){
        var rgb = orig.replace(/\s/g,'').match(/^rgba?\((\d+),(\d+),(\d+)/i);
        return (rgb && rgb.length === 4) ? "" +
        ("0" + parseInt(rgb[1],10).toString(16)).slice(-2) +
        ("0" + parseInt(rgb[2],10).toString(16)).slice(-2) +
        ("0" + parseInt(rgb[3],10).toString(16)).slice(-2) : orig;
    }
    id('btn_fill').addEventListener('click',function(e) {
      for(i=0;i<px;i++) id('ps'+i).style.background = cc;
    });
    id('btn_clear').addEventListener('click',function(e) {
      for(i=0;i<px;i++) id('ps'+i).style.background = 'rgb(0, 0, 0)';
    });
    function send_comm(comm) {
      var xhr=new XMLHttpRequest();
      xhr.onreadystatechange=function() {
        if(xhr.readyState==4 && xhr.status==200) {
          var jd=JSON.parse(xhr.responseText);
          // jd stores the response data
        }
      };
      comm = '.'+comm;
      xhr.open('GET',comm,true);
      xhr.send();
    }
    id('btn_submit').addEventListener('click', function(e) {
      var comm='/cc?pixels=';
      for(i=0;i<px;i++) comm+=rgb2hex(id('ps'+i).style.background);
      send_comm(comm);
    });
  </script>
  <p><a href="."><button class="button button2">Back</button></a></p>
</body>)";
  return page;
}
