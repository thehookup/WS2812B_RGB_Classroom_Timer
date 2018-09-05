/*****************  NEEDED TO MAKE NODEMCU WORK ***************************/
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
/******************  LIBRARY SECTION *************************************/
#include <FastLED.h>
/*****************  LED LAYOUT AND SETUP *********************************/
#define NUM_LEDS 60
/*****************  DECLARATIONS  ****************************************/
CRGB leds[NUM_LEDS];
/*****************  GLOBAL VARIABLES  ************************************/
const int ledPin = 4; //marked as D2 on the board
/*****************  SETUP FUNCTIONS  ****************************************/
void setup() 
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, ledPin, RGB>(leds, NUM_LEDS);
}
/*****************  MAIN LOOP  ****************************************/
void loop() 
{
  delay(5000);
  for(int i = 0; i < 8; i++) 
  {
    Serial.print("leds[");
    Serial.print(i);
    Serial.println("] = CHSV (96,255,192);");
    delay(500);
  }
  delay(100000);
}


