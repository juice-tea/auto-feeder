# include "rfid.h"
#include "ble.h"
#include <Arduino.h>
#include <RTOS.h>

int startScanTime = 0;
int elapsedTime = 0;
int cat = 0; // 0 = No Cat, 1 = Arthur, 2 = Abby
int abbyCount = 0;
int arthurCount = 0;
int localArthurCount = 0;
int localAbbyCount = 0;
int remoteArthurCount = 0;
int remoteAbbyCount = 0;

/// Setup function. ///
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    rfidSetup();
    // while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    // SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);        // Init SPI bus
    // mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    // for (byte i = 0; i < 6; i++) {
    //     key.keyByte[i] = 0xFF;
    // }
}

///Main loop.///
void loop() {
    startScanTime = millis();
    localArthurCount = 0;
    localAbbyCount = 0;
    remoteArthurCount = 0;
    remoteAbbyCount = 0;
    int elapsedTime = millis() - startScanTime;
    while (elapsedTime < 30000) {
        for (int i = 0; i < 10; i++) {
            int cat = rfidScan();
            if (cat == 1) {
                localArthurCount++;
                localAbbyCount = 0;
            }
            else if (cat == 2) {
                localAbbyCount++;
                localArthurCount = 0;
            }
            delay(5);
        }
        mfrc522.PCD_Reset();
        mfrc522.PCD_Init();
        #ifdef DEBUG
        Serial.print(F("Arthur Count: "));
        Serial.println(localArthurCount);
        Serial.print(F("Abby Count: "));
        Serial.println(localAbbyCount);
        #endif
        if (localArthurCount > 10) {
            #ifdef DEBUG
            Serial.println(F("Arthur Detected!"));
            /// When Arthur is detected we will check the BLE service for the presence of Abby at the other feeder. ///
            #endif
            break;
        }
        if (localAbbyCount > 10) {
            #ifdef DEBUG
            Serial.println(F("Abby Detected!"));
            /// When Abby is detected we will toggle the abbyPresence variable True. This variable is broadcasted on the BLE service. ///
            #endif
            break;
        }
    }
}