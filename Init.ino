void InitESPNow() {
  if (esp_now_init() == ESP_OK) {
    //#ifdef EnableSerial
    Serial.println("ESPNow Init Success");
    
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}
void configDeviceAP() {
  String Prefix = SlaveId;
  String Mac = WiFi.macAddress();
  String intStr = String(NODE_NUMBER);
  String Node_str = ":"+intStr;
  String Node_Number=Node_str;
  String SSID = Prefix + Mac;//+ Node_Number;
  String Password = PASS;
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL_SLAVE, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}
void startWifi(){
  if(FRONT_CONEXION){
    WiFi.begin("REDFOR", "HACKFORGOOD");
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

}
