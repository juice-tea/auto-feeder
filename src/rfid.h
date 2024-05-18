#define DEBUG
#define DEBUGFULL

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         25           // Configurable, see typical pin layout above
#define SS_PIN          15            // Configurable, see typical pin layout above
#define MOSI_PIN        13
#define MISO_PIN        12
#define SCK_PIN         14

extern MFRC522 mfrc522;   // Create MFRC522 instance.
extern MFRC522::MIFARE_Key key;

const byte abbyBlock[]    = {                       // Data block if cat = Abby
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x41, 0x62, 0x62, 0x79  
};
const byte arthurBlock[]    = {                     // Data block if cat = Arthur
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x61, 0x72, 
    0x74, 0x68, 0x75, 0x72  
};

const byte sector         = 1;      // Data is accessed at sector 1
const byte blockAddr      = 4;      // Data is accessed at block 4 -> this is where we store the cat name.

int catCheck(const byte *buffer, const byte *dataBlock);

void rfidSetup();

int rfidScan();

void dump_byte_array(byte *buffer, byte bufferSize);