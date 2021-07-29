/*
 * SimpleSender.cpp
 *
 *  Demonstrates sending IR codes in standard format with address and command
 *  An extended example for sending can be found as SendDemo.
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *  MIT License
 */
#include <Arduino.h>

/*
 * Define macros for input and output pin etc.
 */
#include "PinDefinitionsAndMore.h"

//#define SEND_PWM_BY_TIMER
//#define USE_NO_SEND_PWM

#include <IRremote.h>
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  8;      // the number of the LED pin
int aux;
int aux2;
int buttonState = 0;         // variable for reading the pushbutton status
void setup() {
      // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    /*
     * The IR library setup. That's all!
     */
    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin

    Serial.print(F("Ready to send IR signals at pin "));
    Serial.println(IR_SEND_PIN);
}

/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint16_t sAddress = 0x707;
uint8_t sCommand = 0x2;
uint8_t sRepeats = 1;

void loop() {
   buttonState = digitalRead(buttonPin);

   if (buttonState == HIGH) {
    aux = 0;
    while(aux2 <1){
    
    
    Serial.println();
    Serial.print(F("Send now: address=0x"));
    Serial.print(sAddress, HEX);
    Serial.print(F(" command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(" repeats="));
    Serial.print(sRepeats);
    Serial.println();

    Serial.println(F("Send NEC with 16 bit address"));
    Serial.flush();

    // Results for the first loop to: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
    IrSender.sendSamsung(sAddress, sCommand, sRepeats);
    //delay(5000);
    /*
     * If you cannot avoid to send a raw value directly like e.g. 0xCB340102 you must use sendNECRaw()
     */
//    Serial.println(F("Send NECRaw 0xCB340102"));
//    IrSender.sendNECRaw(0xCB340102, sRepeats);
    /*
     * Increment send values
     * Also increment address just for demonstration, which normally makes no sense
     */

    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 4;
    }
    aux2 ++;
    delay(5000);
    }
    //digitalWrite(ledPin, HIGH);
    //delay(5000);
    digitalWrite(ledPin, HIGH);
   }
    else {
      aux2 = 0;
      digitalWrite(ledPin, LOW);
      delay(1000);
      while(aux < 1){
    // turn LED off:
    
      
    /*
     * Print current send values
     */
     
    Serial.println();
    Serial.print(F("Send now: address=0x"));
    Serial.print(sAddress, HEX);
    Serial.print(F(" command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(" repeats="));
    Serial.print(sRepeats);
    Serial.println();

    Serial.println(F("Send NEC with 16 bit address"));
    Serial.flush();

    // Results for the first loop to: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
    IrSender.sendSamsung(sAddress, sCommand, sRepeats);
    //delay(5000);
    /*
     * If you cannot avoid to send a raw value directly like e.g. 0xCB340102 you must use sendNECRaw()
     */
//    Serial.println(F("Send NECRaw 0xCB340102"));
//    IrSender.sendNECRaw(0xCB340102, sRepeats);
    /*
     * Increment send values
     * Also increment address just for demonstration, which normally makes no sense
     */

    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 4;
    }
    aux ++;
    delay(1000);  // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal
}
    }
}
