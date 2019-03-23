#include <esp_now.h>
#include <WiFi.h>
#include <SparkFunBME280.h>
#include <PubSubClient.h>
#define LED_PIN 2
#define FRONT_CONEXION true
// Global copy of slave
#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;

#define CHANNEL_MASTER 3
#define CHANNEL_SLAVE 1
#define DATASIZE 221 //El tamaño de 221 Bytes da espacio para 17 nodos conectados entre si
//DEFINICIONES DE VARIABLES DE NODOS
#define SlaveId "REDFOR"
#define NODE_NUMBER 1
#define PASS "HACKFORGOOD"
int NEIGHTBOUR_NODES[]={2};
struct __attribute__((packed)) SensorData{
  byte Node_Number;
  float temperatura;
  float humedad;
  float presion;
};
SensorData DatosSensor; //Inicializamos el struct
bool newSlave=false;
WiFiClient REDFOR;
PubSubClient client(REDFOR);




void setup() {
  Serial.begin(115200);
  DatosSensor.Node_Number=1;
  pinMode(LED_PIN, OUTPUT);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_MODE_APSTA);
  startWifi();
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  initBME280();
  if(FRONT_CONEXION){
    setupMQTT();  //Esto solo es necesario, y posible si estamos conectados a una red WiFi
  }
  
}

void loop() {
  // In the loop we scan for slave
  ScanForSlave();
  // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  if (SlaveCnt > 0) { // check if slave channel is defined &&newSlave
    // `slave` is defined
    // Add slave as peer if it has not been added already
    manageSlave();
    // pair success or already paired
    // Send data to device
    //sendData();
    sendSensorData();
  } else {
    // No slave found to process
  }

  // wait for 3seconds to run the logic again
  delay(2000);
  leerBME280();
  Serial.print("Humedad: "); Serial.println(DatosSensor.humedad);
  Serial.print("Presion: "); Serial.println(DatosSensor.presion);
  Serial.print("temperatura: ");Serial.println(DatosSensor.temperatura);
  int retry=0;
  do{
    retry++;
    Serial.print("Enviando Dato, Intento Num: ");Serial.println(retry);
  }while(sendSensorData()!=ESP_OK || retry>=3);

}
