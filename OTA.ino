// Setup ------------------------------------------------
void setup_OTA() {
  Serial.println("\nIniciando.");

  // Inicializa SPIFFS
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Ok");
  } else { 
    Serial.println("SPIFFS Falha");
  }

  // Verifica / exibe arquivo
  if (SPIFFS.exists("/Teste.txt")) {
    File f = SPIFFS.open("/Teste.txt", "r");
    if (f) {
      Serial.println("Lendo arquivo:");
      Serial.println(f.readString());
      f.close();
    }
  } else {
    Serial.println("Arquivo não encontrado.");
  }



  /* Permite definir porta para conexão
      Padrão: ESP8266 - 8266
              ESP32   - 3232              */
  //ArduinoOTA.setPort(port);

  /* Permite definir nome do host
      Padrão: ESP8266 - esp8266-[ChipID]
              ESP32   - esp32-[MAC]       */
  // ArduinoOTA.setHostname("host");

  /* Permite definir senha para acesso
      Padrão: sem senha                   */
  // ArduinoOTA.setPassword("senha");

  /* Permite definir senha para acesso via Hash MD5
      Padrão: sem senha                   */
  // ArduinoOTA.setPasswordHash("senhaHashMD5");

  // Define funções de callback do processo
  // Início
  ArduinoOTA.onStart([](){
    String s;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      // Atualizar sketch
      s = "Sketch";
    } else { // U_SPIFFS
      // Atualizar SPIFFS
      s = "SPIFFS";
      // SPIFFS deve ser finalizada
      SPIFFS.end();
    }
    Serial.println("Iniciando OTA - " + s);
  });

  // Fim
  ArduinoOTA.onEnd([](){
    Serial.println("\nOTA Concluído.");
  });

  // Progresso
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.print(progress * 100 / total);
    Serial.print(" ");
  });

  // Falha
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("Erro " + String(error) + " ");
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Falha de autorização");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Falha de inicialização");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Falha de conexão");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Falha de recebimento");
    } else if (error == OTA_END_ERROR) {
      Serial.println("Falha de finalização");
    } else {
      Serial.println("Falha desconhecida");
    }
  });

  // Inicializa OTA
  ArduinoOTA.begin();

  // Pronto
  Serial.println("Atualização via OTA disponível.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Loop --------------------------------------------
void loop_OTA() {
  ArduinoOTA.handle();
}
