// Bibliotecas ------------------ ------------------------
// Bibliotecas para ESP8266
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
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
#include <ESP8266HTTPClient.h>
#include <Time.h>
#include <ESP8266HTTPUpdateServer.h>
// Constantes -------------------------------------------
int count_button = 0;
const char* host = "esp1";
String estado = "ON";
String auth = "";
String emailE = "";
int id = 0;
String nomeMaquina = "";
#define MAX_STRING_LENGTH 50  // Tamanho máximo da string a ser armazenada
#define led_pin 2
//-------------------------------------------------------
WiFiServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  pinMode(D1, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
  Serial.begin(9600);
  setup_conectiveportal();
  setup_OTA();
  ESP.wdtDisable();
  ESP.wdtEnable(0);

  rst_info* resetInfo = ESP.getResetInfoPtr();

  if (resetInfo->reason == REASON_EXT_SYS_RST) {
    Serial.println("Botão de reset pressionado");
    clearEEPROM();
    delay(1000);  // Aguarda um curto período de tempo para permitir a visualização da mensagem
  }
}
void loop() {
  ESP.wdtFeed();
  if (!digitalRead(D1)) {
    count_button++;
    Serial.println("Contagem: ");
    Serial.print(count_button);
    setup_httprequest();

    delay(400);
  }
  ArduinoOTA.handle();
}


void setup_conectiveportal() {
  EEPROM.begin(512);
  WiFi.mode(WIFI_STA);
  WiFiManagerParameter custom_machine_name("machine", "Nome da Máquina", "", 30);
  WiFiManagerParameter custom_email("email", "Email", "", 50);
  WiFiManagerParameter custom_password("password", "Senha", "", 50);

  WiFiManager wifiManager;
  wifiManager.addParameter(&custom_machine_name);
  wifiManager.addParameter(&custom_email);
  wifiManager.addParameter(&custom_password);

  wifiManager.setConfigPortalTimeout(120);

  if (!wifiManager.autoConnect("ESP_ARDUINOECIA", "")) {
    Serial.println(F("Falha na conexao. Resetar e tentar novamente..."));
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    for (int i = 0; i < 5; i++) {
      digitalWrite(led_pin, HIGH);
      delay(500);
      digitalWrite(led_pin, LOW);
      delay(500);
    }


    saveStringToEEPROM(0, String(custom_email.getValue()));
    saveStringToEEPROM(MAX_STRING_LENGTH, String(custom_password.getValue()));
    saveStringToEEPROM(2 * MAX_STRING_LENGTH, String(custom_machine_name.getValue()));

    EEPROM.commit();  // Confirma a gravação na EEPROM

    delay(5000);

    emailE = readStringFromEEPROM(0);
    String passwordE = readStringFromEEPROM(MAX_STRING_LENGTH);
    nomeMaquina = readStringFromEEPROM(2 * MAX_STRING_LENGTH);

    if (emailE == "" || passwordE == "") {
      wifiManager.resetSettings();
      for (int i = 0; i < 5; i++) {
        digitalWrite(led_pin, HIGH);
        delay(500);
        digitalWrite(led_pin, LOW);
        delay(500);
      }
      Serial.println("Redes Wi-Fi limpas");
      clearEEPROM();
      ESP.restart();
    }
    WiFiClient client;
    HTTPClient http;


    String url = "http://192.168.0.66:8090/api/v1/auth/authenticate";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    String body = "{\"email\":\"" + emailE + "\",\"password\":\"" + passwordE + "\"}";
    int httpCode = http.POST(body);

    if (httpCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      String response = http.getString();

      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (error) {
        Serial.println("Error parsing JSON");
      } else {
        const char* token = doc["token"];
        auth = token;
      }
      if (auth == "") {
        for (int i = 0; i < 3; i++) {
          digitalWrite(led_pin, HIGH);
          delay(500);
          digitalWrite(led_pin, LOW);
          delay(500);
        }
        wifiManager.resetSettings();
        clearEEPROM();
        ESP.restart();
      }

    } else {
      Serial.print("Error code: ");
      Serial.println(httpCode);
      // ESP.restart();
    }
    http.end();
    delay(5000);
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}


void saveStringToEEPROM(int address, String value) {
  char charBuf[MAX_STRING_LENGTH];
  value.toCharArray(charBuf, MAX_STRING_LENGTH);
  for (int i = 0; i < value.length(); i++) {
    EEPROM.write(address + i, charBuf[i]);
  }
}

String readStringFromEEPROM(int address) {
  char charBuf[MAX_STRING_LENGTH];
  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    charBuf[i] = EEPROM.read(address + i);
  }
  return String(charBuf);
}


void clearEEPROM() {
  // Inicializa a EEPROM
  EEPROM.begin(512);

  // Limpa a EEPROM escrevendo zeros em todos os endereços
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }

  // Confirma a gravação na EEPROM
  EEPROM.commit();

  // Encerra o uso da EEPROM
  EEPROM.end();
}

void setup_httprequest() {
  digitalWrite(led_pin, HIGH);
  String body = "{\"counter\":" + String(count_button) + ",\"state\":\"" + estado + "\",\"nome\":\"" + nomeMaquina + "\"}";

  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://192.168.0.66:8090/api/v1/nodemcu?email=" + emailE);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(body.length()));
  http.addHeader("Authorization", "Bearer " + auth);
  int httpCode = http.POST(body);

  if (httpCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    String response = http.getString();
    Serial.println("Response: " + response);
    digitalWrite(led_pin, LOW);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpCode);
  }
  http.end();
  delay(100);
}

// Setup ------------------------------------------------
void setup_OTA() {
  ArduinoOTA.setHostname("ESP8266-F");
  ArduinoOTA.begin();
}
