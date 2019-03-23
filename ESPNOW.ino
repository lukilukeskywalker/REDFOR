void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));  //Se borran los exclavos disponibles
  SlaveCnt = 0;
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf(SlaveId) == 0) {
        // SSID of interest
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        slaves[SlaveCnt].channel = CHANNEL_MASTER; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      const esp_now_peer_info_t *peer = &slaves[i];
      const uint8_t *peer_addr = slaves[i].peer_addr;
      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {
          // Pair success
          Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          // How did we get so far!!
          Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          Serial.println("Peer Exists");
        } else {
          Serial.println("Not sure what happened");
        }
        delay(100);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}
bool isScanIN(){  //Esta funcion es para comprobar si ya esta dentro
  
}
int sendSensorData() {
  //String Mensaje=String(compressedData());
  //sprintf((char *)data, Mensaje);
  //data=compressedData();
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *peer_addr = slaves[i].peer_addr;
    uint8_t bs[sizeof(DatosSensor)];
    memcpy(bs, &DatosSensor, sizeof(DatosSensor));
    if (i ==0){
      Serial.print("Enviando: ");Serial.println((char *)bs);
    }
    esp_err_t result = esp_now_send(peer_addr, bs, sizeof(DatosSensor));
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
      return ESP_OK;
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
      return ESP_ERR_ESPNOW_NOT_INIT;
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
      return ESP_ERR_ESPNOW_ARG;
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
      return ESP_ERR_ESPNOW_INTERNAL;
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
      return ESP_ERR_ESPNOW_NO_MEM;
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
      return ESP_ERR_ESPNOW_NOT_FOUND;
    } else {
      Serial.println("Not sure what happened");
      return 80;  
    }
    delay(100);
  }
}

uint8_t data[DATASIZE];
uint64_t pos=0;
void sendDataTest() {
  pos++;
  sprintf((char *)data,"espnow %lld",pos);
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *peer_addr = slaves[i].peer_addr;
    if (i == 0) { // print only for first slave
      Serial.print("Sending: ");
      Serial.println((char *)data);
    }
    esp_err_t result = esp_now_send(peer_addr, data, DATASIZE);
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
    } else {
      Serial.println("Not sure what happened");
    }
    delay(100);
  }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  //Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  digitalWrite(LED_PIN, 1);
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print("\t\tLast Packet Recv from: "); Serial.println(macStr);
  //Serial.print("\t\tLast Packet Recv Data: "); Serial.println((char *)data);
  Serial.print("Hemos recibido: "); Serial.println((char *)data);
  Serial.println("");
  delay(10); // just a little bit
  digitalWrite(LED_PIN, 0);
}
