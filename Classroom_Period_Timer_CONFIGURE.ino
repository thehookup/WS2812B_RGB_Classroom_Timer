
/*****************  NEEDED TO MAKE NODEMCU WORK ***************************/
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
/******************  LIBRARY SECTION *************************************/
#include <FastLED.h>
#include <SimpleTimer.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


/*****************  LED LAYOUT AND SETUP *********************************/
#define NUM_LEDS 57

/*****************  WIFI AND CLOUDMQTT SETUP *****************************/
const char* ssid = "YOURSSID";
const char* password = "YOURWIFIPW";
const char* mqtt_server = "io.adafruit.com";
const int mqtt_port = 1883;
const char *mqtt_user = "YOURMQTTUSER";
const char *mqtt_pass = "YOURMQTTPW";
const char *mqtt_client_name = "UNIQUECLIENTID"; // Client connections cant have the same connection name
/*****************  DECLARATIONS  ****************************************/
WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;
CRGB leds[NUM_LEDS];

/*****************  GLOBAL VARIABLES  ************************************/


const int ledPin = 4; //marked as D2 on the board

int totalMinutes = 0;
int ledsRemaining = 0;
int mappedValue = 0;
int mappedTimer = 10000;
int timerNumber = 1;

bool boot = true;


/*****************  SYSTEM FUNCTIONS  *************************************/

void setup_wifi() 
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() 
{
  // Loop until we're reconnected
  if(!client.connected()) 
  {
    if(WiFi.status() != WL_CONNECTED)
    {
      setup_wifi();
    }
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) 
    {
      Serial.println("connected");
      if(boot == true)
      {
        client.publish("ADAFRUIT_USER/feeds/status","Rebooted");
        boot = false;
      }
      if(boot == false)
      {
        client.publish("ADAFRUIT_USER/feeds/status","Reconnected"); 
      }
      client.subscribe("ADAFRUIT_USER/feeds/bells");
    } 
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  String newTopic = topic;
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  String newPayload = String((char *)payload);
  int intPayload = newPayload.toInt();
  Serial.println(newPayload);
  Serial.println();
  timer.disable(timerNumber);
  delay(10);
  timer.deleteTimer(timerNumber);
  delay(10);
  ledsRemaining = NUM_LEDS;
  totalMinutes = intPayload;
  mappedTimer = totalMinutes / NUM_LEDS * 60000;
  timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
}


/*****************  GLOBAL LIGHT FUNCTIONS  *******************************/

void subtractInterval()
{
  ledsRemaining--;
}


void checkIn()
{
  client.publish("ADAFRUIT_USER/feeds/status","OK");
}

/*****************  SETUP FUNCTIONS  ****************************************/


void setup() 
{
  Serial.begin(115200);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  FastLED.addLeds<WS2812B, ledPin, RGB>(leds, NUM_LEDS);
  
  // GPIO Pin Setup
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  timer.setInterval(90000, checkIn);
  ArduinoOTA.setHostname("classroom227");
  ArduinoOTA.begin(); 
}


/*****************  MAIN LOOP  ****************************************/


void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ledTimer();
  ArduinoOTA.handle();
  timer.run();
  FastLED.show();
}



/****************** BEGIN ANIMATION SECTION ************************/

void ledTimer()
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int colorLED = map(ledsRemaining, 0, NUM_LEDS, 0, 96);
  for(int i = 0; i < ledsRemaining; i++) 
  {
    leds[i] = CHSV (colorLED,255,192);
  }
}

