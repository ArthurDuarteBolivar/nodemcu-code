//void WDT_(unsigned long Duration){
//
//  ESP.wdtDisable();
//  unsigned long prevTime=millis();
//  while (millis()-prevTime<Duration) {
//  ESP.wdtEnable(1000);
//  } 
//}
//
//void setup_watchdog() {
//  Serial.println("----------------------------------------");
//  Serial.println("Esp Inicializado");
//  WDT_(500);
//}
