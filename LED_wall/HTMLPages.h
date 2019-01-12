void sendRoot() {
  Serial.println("Send Root");
  File page = SPIFFS.open("/root.html", "r");
  webserver.streamFile(page, "text/html");
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
      webserver.sendHeader("Location", "/success.html");     // Redirect the client to the success page
      webserver.send(303);
    } else {
      webserver.send(500, "text/plain", "500: couldn't create file");
    }
  }
}
void handleNotFound() { // if the requested file or page doesn't exist, return a 404 not found error
  if (!handleFileRead(webserver.uri())) {        // check if the file exists in the flash memory (SPIFFS), if so, send it
    webserver.send(404, "text/plain", "404: File Not Found");
  }
}

String createSunrisePage() {
  static const String page = "<head><title>LED Wall</title><style>html{ font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                             ".button{ background-color: #195B6A; border: none; color: white; padding: 4px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;width: 400px}"
                             ".button2 {background-color: #0000AA;}</style></head>"
                             "<h3>LED Wall - Sunrise Settings</h3>"
                             "<form action=\"sunrise\" method=\"POST\">\n"
                             "<p>Alarm Time: <input id=\"time\" type=\"time\" name=\"time\" value=\"";
  String hourString = String(sunriseMinuteOfDay / 60);
  String minuteString = String(sunriseMinuteOfDay % 60);
  if (hourString.length() == 1) hourString = "0" + hourString;
  if (minuteString.length() == 1) minuteString = "0" + minuteString;
  String vp = "";
  vp += hourString;
  vp += String(":");
  vp += minuteString;
  vp += String( "\"></p>\n");
  vp += "<p>Sunrise Duration: <input type=\"number\" id=\"duration\" name=\"duration\" min=\"1\" max=\"30\" value=\"";
  vp += sunriseDuration;
  vp += "\"></p>";
  vp += "<p>Sunrise Brightness: <input type=\"number\" id=\"brightness\" name=\"brightness\" min=\"1\" max=\"255\" value=\"";
  vp += sunriseBrightness;
  vp += "\"></p>";
  vp += "<p>Enable Sunrise: <input type=\"checkbox\" id=\"enableSunrise\" name=\"enableSunrise\" value=\"enableSunrise\"";
  if (sunrise) vp += "checked";
  vp += "></p>"
        "<input name=\"Save\" class=\"button\" type=\"submit\" value=\"Save Settings\">"
        "</form>"
        "<p><a href=\".\"><button class=\"button button2\">Back</button></a></p>";

  return page + vp;
}

void sendSunrisePage() {
  webserver.send(200, "text/html  ", createSunrisePage());
}
