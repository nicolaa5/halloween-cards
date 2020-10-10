

/*********
  H A L L O W E E N    P U M P K I N
*********/

/**
 * NOTE: Replace these RFID tag values with the UIDs of your tags
 * If the RFID reader is up and running you can call 'printTagBytes()' 
 * and it will print the 4 UID bytes that identify that tag.
 * 
 * The size of the array depends on how many RFID tags you have. 
 */
byte rfidTagArray[10][4] = {
  {112, 130, 126, 162},
  {80, 173, 119, 162},
  {128, 193, 118, 162},
  {160, 188, 129, 162},
  {32, 218, 125, 162},
  {112, 240, 125, 162},
  {128, 224, 121, 162},
  {96, 30, 126, 162},
  {96, 105, 204, 87},
  {80, 97, 129, 162}
};


//RFID
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   16          // Reset pin
#define SS_PIN    21          // SDA (SS) pin

//PIR
//#include <Wire.h>
#define  LED            2     // Set GPIOs for LED 
#define  MOTION_SENSOR  4     // PIR Motion Sensor

//LED
#include <FastLED.h>
#define LED_PIN     32
#define NUM_LEDS    61
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B   //Check the LED type of your strip and change if needed
#define COLOR_ORDER RBG       //The color order also depends on the strip 
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// Global variables
boolean rfidTagFound = false;
boolean increment = true;
int ledPosition = 2;
int ledRange = 4;
int brightnessCounter = 0;
int rotatingLed = 0;
int ledAnimationDuration = 0;

// Init array that will store new NUID
byte latestFoundTag[4];

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200); //115200 baud is needed for printing RFID values, can be set in your Serial monitor

  //RFID
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Tag Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  //LEDs
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  delay(1);
  
  //LED
  if (rfidTagFound) {
    animateActiveLedStrip();
  }
  else {
    animateIdleLedStrip();
  }

  //RFID
  // Reset the loop if no new tag present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Store Tag bytes into latestFoundTag 
  for (byte i = 0; i < 4; i++) {
    latestFoundTag[i] = mfrc522.uid.uidByte[i];
  }

  int foundTagPosition = foundTagInRFIDList(mfrc522.uid);

  if (foundTagPosition != -1) { // -1 is returned when none of tags in the 'rfidTagArray' matched the found tag
    rfidTagFound = true;
    setLedRange(NUM_LEDS, sizeof(rfidTagArray), foundTagPosition);
    turnLedsOff();
    resetLedSettings();
  }
  
  printTagBytes(mfrc522.uid);
}

/**
  * Returns the RFID tag position in the RFIDTagArray if it was found
  */
int foundTagInRFIDList (MFRC522::Uid foundTag) {
  for (int rfidTag = 0; rfidTag < sizeof(rfidTagArray); rfidTag++) {
    for (int i = 0; i < sizeof(rfidTagArray[rfidTag]); i++) {
      if (foundTag.uidByte[i] != rfidTagArray[rfidTag][i]) {
        return -1;
      }
    }
    return rfidTag;
  }
}

/**
  * Animates the LED strip when an RFID tag has been detected
  */
void animateActiveLedStrip() {
  rotatingLed++;
  if (rotatingLed == (ledPosition + ledRange)) {
    rotatingLed = ledPosition - ledRange;
  }

  leds[rotatingLed] = CRGB::White;
  FastLED.show();
  // clear this led for the next time around the loop
  leds[rotatingLed] = CRGB::Black;

  ledAnimationDuration++;
  if (ledAnimationDuration >= 300) { //
    rfidTagFound = false;
    ledAnimationDuration = 0;
    brightnessCounter = 0;
  }
}

/**
  * Animates the LED strip from low to high brightness when 
  * no RFID tag is detected. 
  */
void animateIdleLedStrip() {
  if (brightnessCounter <= 0) {
    increment = true;
  } else if (brightnessCounter >= 180) {
    increment = false;
  }
  if (increment == true) {
    for ( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;  // Can be any colour.
      leds[i].maximizeBrightness(brightnessCounter); 
    }
    FastLED.show();
    brightnessCounter++ ;  // Increment
  }
  else if (increment == false) {
    for ( int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::White;  // Can be any colour.
      leds[i].maximizeBrightness(brightnessCounter);
    }
    FastLED.show();
    brightnessCounter-- ;  // Decrement
  }
}

/**
  * Turns Leds off. Resetting needs to be done before lighting up a specific LED range
  */
void turnLedsOff () {
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void resetLedSettings() {
  rotatingLed = ledPosition;
  ledAnimationDuration = 0;
}


/**
 Sets the range of LEDs to light up when a RFID tag has been read
 */
void setLedRange(int ledCount, int tagCount, int tagPosition) {
  ledRange = (int)(ledCount / tagCount);
  ledPosition = (int)(tagPosition * ledRange);
}

/**
  * Helper routine to dump a byte array as hex values to Serial.
  */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
  * Use this method for printing the byte values of one of your RFID tags so you can add them to the 'rfidTagArray'
  */
void printTagBytes(MFRC522::Uid tag) {
  Serial.println(F("Found RFID tag:"));
  for (byte i = 0; i < tag.size; i++) {
    Serial.print( tag.uidByte[i]);
  }
  // Dump debug info about the tag; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  if (mfrc522.uid.uidByte[0] != latestFoundTag[0] ||
      mfrc522.uid.uidByte[1] != latestFoundTag[1] ||
      mfrc522.uid.uidByte[2] != latestFoundTag[2] ||
      mfrc522.uid.uidByte[3] != latestFoundTag[3] ) {
    Serial.println(F("A new tag has been detected."));
  }
}
