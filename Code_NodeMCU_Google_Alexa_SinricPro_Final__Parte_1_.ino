/**********************************************************************************
 *  Referencia del código (Créditos al canal:Tech StudyCell)
 *  
 *  TITLE: Google + Alexa + Manual Switch/Button control 4 Relays using NodeMCU & Sinric Pro (Real time feedback)
 *  (flipSwitch can be a tactile button or a toggle switch) (code taken from Sinric Pro examples then modified)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/gpB4600keWA
 *  Related Blog : https://iotcircuithub.com/esp8266-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP8266 NodeMCU : https://github.com/esp8266/Arduino
 *  Download the libraries
 *  ArduinoJson Library: https://github.com/bblanchon/ArduinoJson
 *  arduinoWebSockets Library: https://github.com/Links2004/arduinoWebSockets
 *  SinricPro Library: https://sinricpro.github.io/esp8266-esp32-sdk/
 *  
 *  If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 **********************************************************************************/

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

//Se Incluyen las librerías correspondientes: Para que el Node trabaje con el Arduino IDE, para el ESP8266, y para la utilidad Sinric Pro
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h" 

#include <map>

#define WIFI_SSID         "dlink-EF53"    //Nombre de la Red Wifi a la que se conecta
#define WIFI_PASS         "ayyhp37238"    //Contraseña de la red Wifi a la que se conecta
#define APP_KEY           "668b9182-eca6-4cd4-a147-f662d6ad0c5a"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "06e67430-47e7-4ffc-b850-6e59da73cc7a-88537f07-7875-4c43-9773-d37a4ab1f1d0"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"

//Identificadores de los dispositivos en el broker MQTT de Sinric Pro
#define device_ID_1   "60e47e6f5c24133d62c4637f" 
#define device_ID_2   "60e47ec15c24133d62c46381"
#define device_ID_3   "60e47f5aaf7d763d5b0cdfa0"
#define device_ID_4   "60e530925c24133d62c466a2" 


// Definir los pines (GPIO)del NodeMCU a usar
#define RelayPin1 4  //D2
#define RelayPin2 0  //D3
#define RelayPin3 2 //D4
#define RelayPin4 14 //D5

#define SwitchPin1 10  //SD3
#define SwitchPin2 15   //D8 
#define SwitchPin3 1  //TX
#define SwitchPin4 3   //RX

#define wifiLed   16   //D0

#define TACTILE_BUTTON 1
#define BAUD_RATE   9600 //Tasa de baudios del monitor serie
#define DEBOUNCE_TIME 250 // Tiempo de Respuesta

typedef struct {      // variables para la regla "std::map" de abajo
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;


//Aquí empieza el código principal
//a este código se le pueden añadir N cantidad de dispositivos

// Quiero que mi función map registre y asocie a cada ID único de mis dispositivos, con un pin del relay y un Pin del Switch de etsa forma: {deviceId, {relayPIN,  flipSwitchPIN}}
std::map <String, deviceConfig_t> devices = {  
    {device_ID_1, {  RelayPin1, SwitchPin1 }},
    {device_ID_2, {  RelayPin2, SwitchPin2 }},
    {device_ID_3, {  RelayPin3, SwitchPin3 }},
    {device_ID_4, {  RelayPin4, SwitchPin4 }}     
};

typedef struct {      // Variables para el std::map que relacionará mis dispositivos como sus estados
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    
                                                 

void setupRelays() { 
  for (auto &device : devices) {           // para cada Dispositivo (el cual es una combinación de línea String
    int relayPIN = device.second.relayPIN; // registra y guarda el pin donde se encuentre el pin de los relays
    pinMode(relayPIN, OUTPUT);             // declaro esos pines como salida
    digitalWrite(relayPIN, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices)  {                     // Para cada Dispositivo (que es una combinación String de relay/switch)
    flipSwitchConfig_t flipSwitchConfig;              // transformo mi anterior variable en otra

    flipSwitchConfig.deviceId = device.first;         // Fijamos la varible DeviceID
    flipSwitchConfig.lastFlipSwitchChange = 0;        // Fijamos un tiempo de respuesta
    flipSwitchConfig.lastFlipSwitchState = true;     // set lastFlipSwitchState to false (LOW)--

    int flipSwitchPIN = device.second.flipSwitchPIN;  // obtenemos el pin del switch

    flipSwitches[flipSwitchPIN] = flipSwitchConfig;   // Guardamos la configuración del switch en mi variable de map
    pinMode(flipSwitchPIN, INPUT_PULLUP);                   // Colocamos al Switch como lo que es, un dispositivo de entrada
  }
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  digitalWrite(relayPIN, !state);             // Escribimos el Relé según el estado Encendido o Apagado
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();                                          // Escribimos en una variable, los microsegundos que pasen.
  for (auto &flipSwitch : flipSwitches) {                                         // Para cada FlipSwitch en el comando "Map" de FlipSwitches
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;  // Registra el tiempo de la última activación del switch en variable de tipo long

    if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {                    // Si el tiempo que pasó es mayor al tiempo de respuesta

      int flipSwitchPIN = flipSwitch.first;                                       // Recogemos el flipSwitch pin de al configuración std:map arriba
      bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;           // Llamamos a la variable de tipo long y evaluamos si la condición se cumple o no
      bool flipSwitchState = digitalRead(flipSwitchPIN);                          // Lee el estado actual del Switch
      if (flipSwitchState != lastFlipSwitchState) {                               // Si el estado ha cambiado...
#ifdef TACTILE_BUTTON
        if (flipSwitchState) {                                                    
#endif      
          flipSwitch.second.lastFlipSwitchChange = actualMillis;                  // Registra el tiempo de la nueva activación
          String deviceId = flipSwitch.second.deviceId;                           // Traemos el deviceID de la configuración de arriba
          int relayPIN = devices[deviceId].relayPIN;                              // Traemos el relayPIN de la configuuración de arriba y lo asociamso a su dispositivo
          bool newRelayState = !digitalRead(relayPIN);                            // Devuelve verdadero si es que el pin digital del Node se encuentra en LOW
            digitalWrite(relayPIN, newRelayState);                                // Imprimimos el nuevo estado del relé 
          SinricProSwitch &mySwitch = SinricPro[deviceId];                        // get Switch device from SinricPro
          mySwitch.sendPowerStateEvent(!newRelayState);                           // enviamos el evento a la función para que esta lo registre en la aplicación                                 

#ifdef TACTILE_BUTTON
        }
#endif      
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;                  // actualizamos lastFlipSwitchState
      }
    }
  }
}


//Configuración para conectar el disositivo al WiFi
void setupWiFi()
{
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, LOW);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup()
{
  Serial.begin(BAUD_RATE);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  setupRelays();
  setupFlipSwitches();                          //iniciamos todas las funciones declaradas arriba
  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();                           //Lo que se repetirá constantemente, es el proceso de recibir e interpretar la orden por parte de Sinric Pro
  handleFlipSwitches();
}
