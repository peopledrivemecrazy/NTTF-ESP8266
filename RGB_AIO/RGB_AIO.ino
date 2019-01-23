
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define WLAN_SSID       "Sange & Yasha"
#define WLAN_PASS       "awesomegod321"


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "greedygod"
#define AIO_KEY         "e6c429534481453ebe631cfe9f8c0a65"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

Adafruit_MQTT_Publish custom = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/custom");

Adafruit_MQTT_Subscribe red = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/RGB");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);

  Serial.println(F("Adafruit MQTT demo"));

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&red);

}

uint32_t x = 0;

void loop() {

  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      String condition = (char *)onoffbutton.lastread;
      if (condition == "ON") {
        digitalWrite(D4, HIGH);
      }
      if (condition == "OFF") {
        digitalWrite(D4, LOW);
      }
    }
    if (subscription == &red) {
      Serial.print(F("Slider: "));
      Serial.println((char *)red.lastread);
      String sliderval = ((char *)red.lastread);  // convert to a number

      long long number = strtoll( &sliderval[1], NULL, 16);

      // Split them up into r, g, b values
      int r = number >> 16;
      int g = number >> 8 & 0xFF;
      int b = number & 0xFF;
      int rc =  map(r, 0, 255, 1024, 0);
      int bc =  map(b, 0, 255, 1024, 0);
      int gc =  map(g, 0, 255, 1024, 0);

      Serial.println(rc);
      Serial.println(gc);
      Serial.println(bc);
      analogWrite(D3, rc);
      analogWrite(D2, gc);
      analogWrite(D1, bc);

    }
  }


  /*
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  */
}


void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
