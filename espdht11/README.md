# espdht11
This is a test to connect a ESP-01 to a DHT11 and send the data via wifi to io.adafruit.
Since I dont want to publish credentials, these have to be written in a file called defines.h which must be placedin this directory.

##defines.h
const char* ssid     = "ssid";

const char* password = "password";

must be set.
And also a define called ADAFRUIT_IO_KEY which holds the key must be defined
