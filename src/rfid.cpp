#include "rfid.h"

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Define MFRC522 instance.
MFRC522::MIFARE_Key key; // Define key

void rfidSetup() {
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}

int rfidScan() {
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return 3;

    // // If card is present, select one using PICC_ReadCardSerial()
    if ( ! mfrc522.PICC_ReadCardSerial())
        return 3;

    // Check for compatibility
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        #ifdef DEBUG
        Serial.println(F("Card is not compatible with this code."));
        #endif
        return 0;
    }

    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    // Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        #ifdef DEBUG
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        #endif
        return 0;
    }

    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        #ifdef DEBUG
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        #endif
    }
    // dump_byte_array(buffer, 16); Serial.println();
    int cat = catCheck(buffer, arthurBlock);
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    return cat;
}

int catCheck(const byte *buffer, const byte *dataBlock) {
    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    byte arthurCount = 0;
    byte abbyCount = 0;
    for (byte i = 0; i < 16; i++) {
        if (buffer[i] == arthurBlock[i])
            arthurCount++;
        if (buffer[i] == abbyBlock[i])
            abbyCount++;
    }
    if (arthurCount == 16) {
        #ifdef DEBUG
        // Serial.println(F("Arthur Detected!"));
        #endif
        return 1;
    } 
    else if (abbyCount == 16) {
        #ifdef DEBUG
        // Serial.println(F("Abby Detected!"));
        #endif
        return 2;
    }
    else {
        return 0;
    }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}