ICACHE_RAM_ATTR void count_tube(){   // Функция вызываемая прерыванием, регистрация импульсов трубки
     bCP10s++;
     bCP1m++;
     bCP5m++;
     bCP60m++; 
}

void timers(){
   unsigned long currentMillis = millis();  
     if((currentMillis - t10s) > 10000 || currentMillis < t10s){ // Событие срабатывающее каждые 10 сек 
            val10s  = bCP10s*0.06*settings.tubecof;
            t10s    = currentMillis;
            CP10s   = bCP10s;
            bCP10s  = 0;  
        }
        
    if((currentMillis - t60s) > 60000 || currentMillis < t60s){ // Событие срабатывающее каждые 60 сек 
             val1m  = bCP1m*0.01*settings.tubecof;
             t60s   = currentMillis;
             CP1m   = bCP1m;
             bCP1m  = 0;     
        }

    if((currentMillis - t5min) > 300000 || currentMillis < t5min){ // Событие срабатывающее каждые 5 мин
              val5m = bCP5m*0.002*settings.tubecof;
              t5min = currentMillis;
              CP5m  = bCP5m;
              bCP5m = 0; 
        }    
     
    if((currentMillis - t60min) > 3600000 || currentMillis < t60min){ // Событие срабатывающее каждые 60 мин
             val60m = bCP60m*0.00017*settings.tubecof;
             t60min = currentMillis;
             CP60m  = bCP60m;
             bCP60m = 0;
        }  
  
}

void alarm(){
     unsigned long currentMillis = millis();
       if(currentMillis - previousMillis > 1000 or previousMillis == 0 ) { 
           tone(BUZZER, 2000);
           digitalWrite(ALARM_LED, HIGH);
          }
       if(currentMillis - previousMillis > 2000 or previousMillis == 0 ) {
           noTone(BUZZER);
           digitalWrite(ALARM_LED, LOW);
           previousMillis = currentMillis; 
        }
  }

void led_ind(){
    if(!digitalRead(TUBE)){
          led_stat = true;
          digitalWrite(STATUS_LED, HIGH);
          currentMillisLed = millis();
           }
     if(millis() - currentMillisLed > 100 && led_stat ){
                led_stat = false;
                digitalWrite(STATUS_LED, LOW);
          }     
       
}
