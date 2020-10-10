# Halloween card seek game
Halloween 'seek the RFID cards' game! Hide RFID cards in the room for people to find. When a card is found scan it with the RFID reader to claim the price indicated by the LED strip. 

<img src="/images/halloween-cards.gif" alt="Halloween RFID card game" width="360" height="240">

## Hardware

For this project you need:
* Arduino, ESP32 or other board that can run Arduino code
* An RFID reader (the one used in the project is the RC522, with the MFRC522 library)
* RFID tags/cards (supported tags: mifare1 S50, mifare1 S70 MIFARE Ultralight, mifare Pro, MIFARE DESFire)
* LED strip (type used in project: WS2812B) 
* Breadboard and jumper cables

Check the amount of LEDs you want to include in your project, you might need an external power source that can provide ~2A if you want to power many LEDs.
This project uses a strip with around 60 LEDs for which the brightness is sufficient in a darker room.

<img src="/images/hardware.png" alt="Components" width="403" height="302">

You can see the main components used in this project in this link.
https://www.circuito.io/app?components=513,216577,360217,761981


## Software
Run the **Pumpkin.ino** file on your Arduino compatible board. 
Define the right pins for communicating with your RFID reader and LED strip (The pins you have to use will change based on the board you will be using).

The LED strip is controlled using the **FastLED** library: https://github.com/FastLED/FastLED

The RFID reader communicates over SPI using a library created by Miguel Balboa:  https://github.com/miguelbalboa/rfid

The pinout for the RFID reader for most arduino boards is specified here: 

``` Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
```
