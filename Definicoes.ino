

// Bibliotecas ------------------ ------------------------
  // Bibliotecas para ESP8266
  #include <EEPROM.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <FS.h>
  #include <WiFiManager.h>
  #include <ESP.h>
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
// Bibliotecas comuns
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// Constantes -------------------------------------------
int count_button = 0;
int estado = 1;  
int estadoDesligado = 0;  
char estadoMaquina[15] = "Ligado";  
int horas = 0;
int minutos = 0;
int segundos = 0;
int decimas = 0;
long milisegundos = 0;
int verifica = 0;
//-------------------------------------------------------

void setup() {
      pinMode(D1, INPUT_PULLUP);
      Serial.begin(115200);
      setup_conectiveportal();
      setup_OTA();
//      setup_watchdog();

}
void loop(){
          if (!digitalRead(D1)){
    count_button++;
    setup_httprequest();
    Serial.println(count_button);
    delay(400);
          }

      
  loop_OTA();
}
