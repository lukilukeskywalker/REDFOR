#define IpMqtt "192.168.10.100"
void setupMQTT() {
  client.setServer(IpMqtt, 1883);   //se abre el portal MQTT
  client.setCallback(Recepcion);
//  mqttClient.setCallback(callback);
}
void Recepcion(char* tema,byte* carga, unsigned int length){    //MQTT
  Serial.println("Hemos recibido algo... que para esta aplicacion, todavia no deeria suceder");
}
void ServerManager(){ //Se ocupa de mantener la conexion con el servidor y de cumplir //Uff tengo que buscar el timeout de mqtt, porque si no avisa repite
  if(!REDFOR.connected()){ //Si no esta conectado, nos intentamos conectar
    if(client.connect("REDFOR")){  //boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage) //Claves: puerta, ae7elh0st
      client.publish("REDFOR","a su servicio");
      
    }
  }
  else{ //Si ya esta conectado, que se ocupe de las tareas del servidor
    client.loop();
  }
}
