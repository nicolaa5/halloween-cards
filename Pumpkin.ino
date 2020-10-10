

/*********
  H A L L O W E E N    P U M P K I N    
*********/

//  RFID TAG UIDs
//  70 82 7E A2
//  50 AD 77 A2
//  80 C1 76 A2
//  A0 BC 81 A2
//  20 DA 7D A2
//  70 F0 7D A2
//  80 E0 79 A2
//  60 1E 7E A2
//  60 69 CC 57
//  50 61 81 A2

//  112 130 126 162
//  80 173 119 162
//  128 193 118 162
//  160 188 129 162
//  32 218 125 162
//  112 240 125 162
//  128 224 121 162
//  96 30 126 162
//  96 105 204 87
//  80 97 129 162

byte A[] = {112, 130, 126, 162};
byte B[] = {80, 173, 119, 162};
byte C[] = {128, 193, 118, 162};
byte D[] = {160, 188, 129, 162};
byte E[] = {32, 218, 125, 162};
byte F[] = {112, 240, 125, 162};
byte G[] = {128, 224, 121, 162};
byte H[] = {96, 30, 126, 162};
byte I[] = {96, 105, 204, 87};
byte J[] = {80, 97, 129, 162};

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

//byte A[] = {70, 82, 7E, A2};

//PIR
//#include <Wire.h>
#define  LED            2     // Set GPIOs for LED 
#define  MOTION_SENSOR  4     // PIR Motion Sensor

//LED
#include <FastLED.h>
#define LED_PIN     32
#define NUM_LEDS    61
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B //WS2811
#define COLOR_ORDER RBG //GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// Global variables
unsigned long now = millis();
unsigned long previousTimestamp = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
boolean rfidTagFound = false;
boolean increment = true;
int ledPosition = 2;
int ledRange = 4; 
int brightnessCounter = 0;
int rotatingLed = 0;
int showLocationCount = 0;

// Init array that will store new NUID 
byte nuidPICC[4];

 // Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN); 


// Checks if motion was detected, sets LED HIGH and starts a timer
void IRAM_ATTR detectsMovement() {
  Serial.println("Motion detected");
  digitalWrite(LED, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  //PIR
  pinMode(MOTION_SENSOR, INPUT_PULLUP);  // PIR Motion Sensor mode INPUT_PULLUP
  attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR), detectsMovement, RISING); // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Set LED to LOW

  //RFID
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  //LEDs
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  delay(1); 
  // PIR 
  now = millis(); // Current time  
  if(startTimer && (now - lastTrigger > 5000)) { // Turn off the LED after the number of seconds defined in the timeSeconds variable
    Serial.println("Motion stopped...");
    digitalWrite(LED, LOW);
    startTimer = false;
  }

  //LED
  if (rfidTagFound) { 
    rotatingLed++;
    if (rotatingLed == (ledPosition + ledRange)) {
      rotatingLed = ledPosition - ledRange; 
    }
    
    leds[rotatingLed] = CRGB::White;
    FastLED.show();
    // clear this led for the next time around the loop
    leds[rotatingLed] = CRGB::Black;

    showLocationCount++;
    if (showLocationCount >= 300) {
      rfidTagFound = false;
      showLocationCount = 0;
      brightnessCounter = 0;
    }
  }
  else {
    if (brightnessCounter <= 0) {
      increment = true;
    } else if (brightnessCounter >= 180) {
      increment = false;
    }
    if (increment == true) {
      for ( int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::White;  // Can be any colour.
          leds[i].maximizeBrightness(brightnessCounter);  // 'FastLED_fade_counter' How low we want to fade down to 0 = maximum.
        }
      FastLED.show();
      brightnessCounter++ ;  // Increment
    }
    else if (increment == false) {
      for ( int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB::White;  // Can be any colour.
          leds[i].maximizeBrightness(brightnessCounter);  // 'FastLED_fade_counter' How low we want to fade down to 0 = maximum.
        }
      FastLED.show();
      brightnessCounter-- ;  // Decrement
    }

  }

  //RFID
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
    
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = mfrc522.uid.uidByte[i];
  }

  int foundTagPosition = foundTagInRFIDList();
  
  if (foundTagPosition != -1) {
    rfidTagFound = true;
    setLEDRange(NUM_LEDS, sizeof(rfidTagArray), foundTagPosition)
    turnLedsOff();
  }
}

int foundTagInRFIDList () {
  for (int rfidTag = 0; rfidTag < sizeof(rfidTagArray); rfidTag++) {
    for (int i = 0; i < sizeof(rfidTagArray[rfidTag]); i++) {
      if(mfrc522.uid.uidByte[i] != rfidTagArray[rfidTag][i]) {
        return -1; 
      }
    }
    return rfidTag; 
  }
}

void turnLedsOff () {
  for ( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black; 
  }
  FastLED.show();
  rotatingLed = ledPosition;  
  showLocationCount = 0; 
}

void setLEDRange(int ledCount, int cardCount, int cardPosition) {
   ledRange = (int)(ledCount / cardCount);
   ledPosition = (int)(cardPosition * ledRange); 
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

void printCard() {
    Serial.println(F("The NUID tag is:"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print( mfrc522.uid.uidByte[i]);
    }
    // Dump debug info about the card; PICC_HaltA() is automatically called
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); 
  
    if (mfrc522.uid.uidByte[0] != nuidPICC[0] || 
      mfrc522.uid.uidByte[1] != nuidPICC[1] || 
      mfrc522.uid.uidByte[2] != nuidPICC[2] || 
      mfrc522.uid.uidByte[3] != nuidPICC[3] ) {
      Serial.println(F("A new card has been detected."));
    }
}
