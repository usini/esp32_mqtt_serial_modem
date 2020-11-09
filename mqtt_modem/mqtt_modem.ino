#include "secrets.h"

#ifdef ESP32
  #include <WiFiMulti.h>
  #include <ESPmDNS.h>
  #include <WiFiClientSecure.h>
  WiFiMulti wifiMulti; // WiFiMulti is used to try to connect to multiples SSID
  WiFiClientSecure client_secure;
#endif

#ifdef ESP8266
  #include "SoftwareSerial.h"
  #include <WiFiClientSecure.h>
  #include "WiFiClientSecureAxTLS.h"
  #include <ESP8266WifiMulti.h>
  #include <ESP8266mDNS.h>
  ESP8266WiFiMulti wifiMulti; // WiFiMulti is used to try to connect to multiples SSID
  WiFiClientSecure client_secure;
#endif

#include <PubSubClient.h>
void callback(char* topic, byte* payload, unsigned int length);
bool local = true;
String line = "";

WiFiClient client;
PubSubClient mqtt_local(client);
PubSubClient mqtt_remote(client_secure);
//Adafruit_MQTT_Subscribe rx_sub_local = Adafruit_MQTT_Subscribe(&mqtt_local, SUBSCRIBE_TOPIC);
//Adafruit_MQTT_Subscribe rx_sub_remote = Adafruit_MQTT_Subscribe(&mqtt_remote, SUBSCRIBE_TOPIC);
void setup() {
  Serial.println(" ");
  Serial.begin(115200);

  #ifdef ESP32
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  #endif

  MDNS.begin(NAME);

  wifiMulti.addAP(LOCAL_SSID, LOCAL_PASS); // Add WiFi1 (stored from settings)
  wifiMulti.addAP(REMOTE_SSID, REMOTE_PASS); // Add WiFi2 (stored from settings)

  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("AT+WIFI");
    #ifdef ESP32
      Serial2.println("AT+WIFI");
    #endif
    delay(1000);
  }
  #ifdef ESP32
  Serial.print("AT+");
  Serial.print(WiFi.SSID());
  #endif

  if(WiFi.SSID() == REMOTE_SSID) {
    local = false;
    Serial.println("AT+REMOTE");
    #ifdef ESP32
      Serial2.println("AT+REMOTE");
      client_secure.setCACert(rootCACertificate);
    #endif

    #ifdef ESP8266
      client_secure.setInsecure();
    #endif

  }

  if(local) {
    Serial.println("AT+LOCAL");
    #ifdef ESP32
      Serial2.println("AT+LOCAL");
    #endif
    mqtt_local.setServer(LOCAL_MQTT_SERVER, 1883);
    mqtt_local.setCallback(callback);
  } else {
    Serial.println("AT+REMOTE");
    #ifdef ESP32
      Serial2.println("AT+REMOTE");
    #endif
    mqtt_remote.setServer(REMOTE_MQTT_SERVER, 8883);
    mqtt_remote.setCallback(callback);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print("/");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("AT+CONNECTING");
    // Create a random client ID
    String clientId = "ESP8266Modem-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if(local) {
      if (mqtt_local.connect(clientId.c_str(), LOCAL_MQTT_USERNAME, LOCAL_MQTT_KEY)) {
        Serial.println("AT+CONNECTED");
        mqtt_local.subscribe(SUBSCRIBE_TOPIC);
      } else {
        delay(1000);
      }
    } else {
      if (mqtt_remote.connect(clientId.c_str(), REMOTE_MQTT_USERNAME, REMOTE_MQTT_KEY)) {
        Serial.println("AT+CONNECTED");
        mqtt_remote.subscribe(SUBSCRIBE_TOPIC);
      } else {
        delay(1000);
      }
    }
  }
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void send(String line) {
    for(int i = 0; i < TOPIC_LIMIT; i++) {
      String value = getValue(line, '/', i);

      if(value == "") {
          if(i == 0) {
            Serial.print("INVALID:");
            Serial.println(line);
          } else {
            String message = getValue(line, '/', i - 1);
            String topic = line.substring(0, line.length() - message.length());
            bool published = false;
            /*
            Serial.print("MESSAGE : ");
            Serial.println(message);
            Serial.print("TOPIC : ");
            Serial.println(topic);
            */
            if(local) {
              published = mqtt_local.publish(topic.c_str(), message.c_str());
            } else {
              published = mqtt_remote.publish(topic.c_str(), message.c_str());
            }

            if(published) {
              Serial.println("AT+OK");
              #ifdef ESP32
                Serial2.println("AT+OK");
              #endif
            } else {
              #ifdef ESP32
                Serial.println("AT+FAIL");
              #endif
            }
          }
          break;
      }
    }
  }

void serialRead() {
  if(Serial.available() > 0) {
      String line = Serial.readStringUntil('\n');
      //Serial.println("AT+SERIAL");
      send(line);
    }
    #ifdef ESP32
      if(Serial2.available() > 0) {
        String line = Serial2.readStringUntil('\n');
        //Serial.println("AT+SERIAL2");
        send(line);
      }
    #endif
}


void loop() {
  serialRead();
  if(local) {
    if(!mqtt_local.connected()){
      reconnect();
    }
    mqtt_local.loop();
  } else {
    if(!mqtt_remote.connected()){
      reconnect();
    }
    mqtt_remote.loop();
  }
}

