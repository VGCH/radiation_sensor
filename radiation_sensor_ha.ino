/* Устройство для мониторинга уровня фоновой радиации
 *  
 *  © CYBEREX Tech, 2019
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266SSDP.h>
#include <PubSubClient.h>
#include "const_js.h"

// Pins
#define TUBE           4               // Пин сигнала детектора
#define BUZZER         5               // Пин звукового оповещения
#define STATUS_LED     1               // Индикатор статуса системы / индикация регистрации частиц
#define ALARM_LED      3               // Индикатор высокого уровня излучения
#define SYSTEM         16              // Режим восстановления доступа


// WEBs
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
IPAddress apIP(10, 10, 20, 1);
IPAddress netMsk(255, 255, 255, 0);
ESP8266HTTPUpdateServer httpUpdater;

// DNS server
const byte             DNS_PORT = 53;
DNSServer              dnsServer;

// Текущий статус WLAN
unsigned int status = WL_IDLE_STATUS;

// Переменные для хранения статуса сетевого подключения
boolean connect;
boolean wi_fi_st;

boolean stat_reboot, led_stat; 

// Переменные используемые для CaptivePortal
const char *myHostname  = "Cyberex_sensors";           // Имя создаваемого хоста Cyberex_sensor.local 
const char *softAP_ssid = "CYBEREX-R-SENS";            // Имя создаваемой точки доступа Wi-Fi

String version_code = "R-1.5.11.23";

// Переменная для хранения времени последней попытки сетевого подключения
unsigned long lastConnectTry = 0;
//Другие таймеры
unsigned long reboot_t = 0;
uint32_t t10s,  t60s, t5min, t60min, previousMillis, lastMsg, currentMillisLed;

int count_rf = 0;

// Структура для хранения токенов сессий 
struct Token {
    String value;
    long created;
    long lifetime;
};

Token   tokens[20];                    // Длина массива структур хранения токенов 

#define TOKEN_LIFETIME 6000000         // время жизни токена в секундах

#define MAX_STRING_LENGTH 30           // Максимальное количество символов для хранения в конфигурации


// Структура для хранения конфигурации устройства
struct {
     boolean buzzer_en;
     boolean mqtt_en;
     boolean json_en;
     int     mqtt_time;
     int     statteeprom; 
     int     tubecof;
     char    mySSID[MAX_STRING_LENGTH];
     char    myPW[MAX_STRING_LENGTH];
     char    mqtt_serv[MAX_STRING_LENGTH];
     String  mqtt_user;
     String  mqtt_passw;
     String  mqtt_id;
     String  mqtt_topic;
     String  passwd;    
  } settings;


int  CP10s, CP1m, CP5m, CP60m;                                      // Накопление импульсов трубки 10с/1м/5м/60м 
int  bCP10s, bCP1m, bCP5m, bCP60m;                                      // Накопление импульсов трубки 10с/1м/5м/60м буфер
float val10s, val1m, val5m, val60m;                                 // Уровень радиации 10с/1м/5м/60м 

void setup() {
    EEPROM.begin(sizeof(settings));                                 // Инициализация EEPROM в соответствии с размером структуры конфигурации
    pinMode(BUZZER,     OUTPUT);         
    pinMode(STATUS_LED, OUTPUT);
    pinMode(ALARM_LED,  OUTPUT);
    pinMode(TUBE,        INPUT);
    read_config();                                                  // Чтение конфигурации из EEPROM
    check_clean();                                                  // Провека на запуск устройства после первичной прошивки
     if(String(settings.passwd) == ""){   
        settings.passwd = "admin";                                  // Если  переменная settings.passwd пуста, то назначаем пароль по умолчанию
     }
     WiFi.mode(WIFI_STA);                                           // Выбираем режим клиента для сетевого подключения по Wi-Fi                
     WiFi.hostname("Radiation-Sensor [CYBEREX TECH]");              // Указываеем имя хоста, который будет отображаться в Wi-Fi роутере, при подключении устройства
     if(WiFi.status() != WL_CONNECTED) {                            // Инициализируем подключение, если устройство ещё не подключено к сети
           WiFi.begin(settings.mySSID, settings.myPW);
      }

     for(int x = 0; x < 60; x ++){                                  // Цикл ожидания подключения к сети (30 сек)
          if (WiFi.status() == WL_CONNECTED){ 
                digitalWrite(ALARM_LED, LOW);
                break;
           }
               digitalWrite(ALARM_LED, !digitalRead(ALARM_LED));     // Мигаем светодиодом в процессе ожидания подключения
               delay(500); 
          }

     if(WiFi.status() != WL_CONNECTED) {                             // Если подключение не удалось, то запускаем режим точки доступа Wi-Fi для конфигурации устройства
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAPConfig(apIP, apIP, netMsk);
            WiFi.softAP(softAP_ssid);
            digitalWrite(ALARM_LED,  HIGH);
        }
        
        delay(2000);
        // Запускаем DNS сервер
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", apIP);
        // WEB страницы
        server.on("/", page_process);
        server.on("/login", handleLogin);
        server.on("/script.js", reboot_devsend);
        server.on("/style.css", css);
        server.on("/index.html", HTTP_GET, [](){
        server.send(200, "text/plain", "IoT Radiation Sensor"); });
        server.on("/description.xml", HTTP_GET, [](){SSDP.schema(server.client());});
        server.on("/inline", []() {
        server.send(200, "text/plain", "this works without need of authentification");
        });
        server.onNotFound(handleNotFound);
        // Здесь список заголовков для записи
        const char * headerkeys[] = {"User-Agent", "Cookie"} ;
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
        // Запускаем на сервере отслеживание заголовков 
        server.collectHeaders(headerkeys, headerkeyssize);
        server.begin();
        connect = strlen(settings.mySSID) > 0;                               // Статус подключения к Wi-Fi сети, если есть имя сети
        SSDP_init();
        attachInterrupt(digitalPinToInterrupt(4), count_tube, FALLING);     // Задействуем аппаратное прерывание для изменения импульсов трубки
      
}

void loop() {
        portals();
        dnsServer.processNextRequest();
        server.handleClient();  
        if(val10s > 100){
               alarm();
              }else{
                 noTone(BUZZER);
                 digitalWrite(ALARM_LED, LOW);
        if(settings.buzzer_en){ digitalWrite(BUZZER, !digitalRead(TUBE)); }
          }
        led_ind();
        timers();
        reboot_dev_delay();
        MQTT_send();
}

void reboot_devsend(){
   server.send(200, "text", reb_d);
}
