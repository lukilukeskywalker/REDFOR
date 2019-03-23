//Definimos los pines SPI
#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5

BME280 bme280;
void leerBME280(){
  DatosSensor.temperatura=bme280.readTempC();
  DatosSensor.humedad=bme280.readFloatHumidity();
  DatosSensor.presion=bme280.readFloatPressure();
  
}
void initBME280(){
  bme280.settings.commInterface=SPI_MODE;
  bme280.settings.chipSelectPin=CS;
  bme280.settings.runMode=3;
  bme280.settings.tStandby=0;
  bme280.settings.filter=0;
  bme280.settings.tempOverSample=1;
  bme280.settings.pressOverSample=1;
  bme280.settings.humidOverSample=1;
  Serial.print("Iniciando Sensor enviriomental");
  Serial.println(bme280.begin(),HEX);
  
}
char compressedData(){
  char Mensaje[13]; //Array de char con tamaño 13.
  byte bytesT[4];
  memcpy(bytesT, (unsigned char*) (&DatosSensor.temperatura), 4);
  byte bytesP[4];
  memcpy(bytesP, (unsigned char*) (&DatosSensor.presion), 4);
  byte bytesH[4];
  memcpy(bytesP, (unsigned char*) (&DatosSensor.humedad), 4);
  for(int i=0; i<4; i++){
    Mensaje[1+i]=bytesT[i];
  }
  for(int i=0; i<4; i++){
    Mensaje[4+i]=bytesP[i];
  }
  for(int i=0; i<4; i++){
    Mensaje[8+i]=bytesH[i];
  }
  Mensaje[0]=NODE_NUMBER;
  return *Mensaje;  //devolvemos el mensaje, con los datos
}
