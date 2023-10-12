// Функция отправки данных по протоколу MQTT
void MQTT_send(){
  if(settings.mqtt_en){
   unsigned long nows = millis();
  if (nows - lastMsg > (settings.mqtt_time*1000) && (settings.mqtt_time*1000) > 999) {
    if(WiFi.status() == WL_CONNECTED) {
      client.setServer(settings.mqtt_serv, 1883);
      client.loop();
      String top = settings.mqtt_topic+"/jsondata";
      String topics[] = { String(settings.mqtt_topic)+"/CP10s", String(settings.mqtt_topic)+"/CP1m", String(settings.mqtt_topic)+"/CP5m", String(settings.mqtt_topic)+"/CP60m", String(settings.mqtt_topic)+"/val10s", String(settings.mqtt_topic)+"/val1m", String(settings.mqtt_topic)+"/val5m", String(settings.mqtt_topic)+"/val60m"};
      String data[]   = { String(CP10s), String(CP1m), String(CP5m), String(CP60m), String(val10s), String(val1m), String(val5m), String(val60m)};

     if(client.connected()){
       count_rf = 0;
          if(settings.json_en){
             client.publish(top.c_str(), JSON_DATA().c_str());
          }else{
              for (int i = 0; i < 8; i++) {
                  client.publish(topics[i].c_str(), data[i].c_str());
                }
            }
       }else{
        count_rf++;
        if (client.connect(settings.mqtt_id.c_str(), settings.mqtt_user.c_str(), settings.mqtt_passw.c_str())){
          if(settings.json_en){           
          client.publish(top.c_str(), JSON_DATA().c_str());
            }else{             
          for (int i = 0; i < 8; i++) {
               client.publish(topics[i].c_str(), data[i].c_str());
              }    
            }
          }else{
            if(count_rf > 2){
                WiFi.disconnect();
                delay(500);
                WiFi.begin(settings.mySSID, settings.myPW);
                count_rf = 0;
            }
          }
        }
      }
   lastMsg = nows; 
  }
 }
}

String MQTT_status(){
    String response_message = "";
    if(client.connected()){
       response_message = "подключен";
      }else{
       response_message = "отключен";
    }
    return response_message;
} 
