/***************************************************
  Adafruit ESP8266 Sensor Module
  
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board:
  ----> https://www.adafruit.com/product/2471
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#define DO_ADAFRUIT_PUBLISHING

// Libraries
#include <ESP8266WiFi.h>
#ifdef DO_ADAFRUIT_PUBLISHING
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Adafruit_IO_Client.h"
#endif
#include "DHT.h"
#include "defines.h"

#define CNT 15
int temperaturen[CNT];
int feuchten[CNT];
int i=0;

// DHT 11 sensor
#define DHTPIN 2
#define DHTTYPE DHT11 
// DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

#ifdef DO_ADAFRUIT_PUBLISHING
WiFiClient client;
Adafruit_IO_Client aio = Adafruit_IO_Client(client, ADAFRUIT_IO_KEY);
Adafruit_IO_Feed temperatureFeed = aio.getFeed("Temperatur");
Adafruit_IO_Feed humidityFeed = aio.getFeed("Luftfeuchtigkeit");
#endif

int loopcount=0;

/*************************** Sketch Code ************************************/
void setup() {
  // Init sensor
  dht.begin();

  Serial.begin(115200);
  Serial.println("");
  Serial.println(F("Christians Klimalogger"));
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#ifdef DO_ADAFRUIT_PUBLISHING
  // Initialize the Adafruit IO client class (not strictly necessary with the
  // client class, but good practice).
  aio.begin();
#endif
}

void add_values(int temp, int humid)
{
  static int buffer_full=0;

  if(buffer_full==0)
  {
    int i;
    for(i=0;i<CNT;i++)
    {
      temperaturen[i]=temp;
      feuchten[i]=humid;
    }
    buffer_full=1;
  }
  //Serial.print(temp);
  //Serial.print(humid);
  //Serial.print(i);
  temperaturen[i]=temp;
  feuchten[i++]=humid;
  if(i>=CNT)
  {
    i=0;
  }
}

int get_temperature(void)
{
  int tempsumme=0;
  int result;
  int i;
  for(i=0;i<CNT;i++)
  {
    tempsumme+=temperaturen[i];
  }
  result=tempsumme/CNT;
  return result;
}

int get_humidity(void)
{
  int humsumme=0;
  int result;
  int i;
  for(i=0;i<CNT;i++)
  {
    humsumme+=feuchten[i];
  }
  result=humsumme/CNT;
  return result;
}

void loop() {

  int temperature_data = (int)dht.readTemperature();
  int humidity_data = (int)dht.readHumidity();
  
  if((humidity_data==2147483647)||(temperature_data==2147483647)||((temperature_data==0)&&(humidity_data==0))){
      Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      Serial.print("Fehlmessung: ");
      Serial.print(humidity_data);
      Serial.print(" / ");
      Serial.println(temperature_data);
      Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }else{
    add_values(temperature_data,humidity_data);
    Serial.println("-------------------------------------------------------------------------------");
    Serial.print("Lufttemperatur           : ");
    Serial.print(temperature_data);
    Serial.println(" Grad Celsius");
    Serial.print("Luftfeuchte              : ");
    Serial.print(humidity_data);
    Serial.println(" %");
    Serial.print("Lufttemperatur(gemittelt): ");
    Serial.print(get_temperature());
    Serial.println(" Grad Celsius");
    Serial.print("Luftfeuchte(gemittelt)   : ");
    Serial.print(get_humidity());
    Serial.println(" %");
#ifdef DO_ADAFRUIT_PUBLISHING
    if(loopcount++>=CNT)
    {
      loopcount=0;
      Serial.println("Publishing Values to Adafruit IO");
      Serial.print("First the temperature ");
      if (!temperatureFeed.send(get_temperature())) 
      {
          Serial.println(F("*** Error writing value to temperatureFeed!"));
      }
      Serial.println("and than the humidity");
      if (!humidityFeed.send(get_humidity())) 
      {
          Serial.println(F("*** Error writing value to FeuchteFeed!"));
      }
#endif    
    }
  }
  delay(60000);
}


