#include "secrets.h"
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <WiFiClientSecure.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

bool local = true;
String line = "";
WiFiMulti wifiMulti; // WiFiMulti is used to try to connect to multiples SSID
WiFiClient client;
WiFiClientSecure client_secure;
Adafruit_MQTT_Client mqtt_local(&client, LOCAL_MQTT_SERVER, LOCAL_MQTT_SERVERPORT, LOCAL_MQTT_USERNAME, LOCAL_MQTT_KEY);
Adafruit_MQTT_Client mqtt_remote(&client_secure, REMOTE_MQTT_SERVER, REMOTE_MQTT_SERVERPORT, REMOTE_MQTT_USERNAME, REMOTE_MQTT_KEY);

void setup() {
  Serial.println(" ");
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  MDNS.begin(NAME);

  wifiMulti.addAP(LOCAL_SSID, LOCAL_PASS); // Add WiFi1 (stored from settings)
  wifiMulti.addAP(REMOTE_SSID, REMOTE_PASS); // Add WiFi2 (stored from settings)
  
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WIFI");
    Serial2.println("WIFI");
    delay(1000);
  }
  
  Serial.println(WiFi.SSID());
  if(WiFi.SSID() == REMOTE_SSID) {
    local = false;
    Serial.println("REMOTE");
    Serial2.println("REMOTE");
    client_secure.setCACert(rootCACertificate);
  }

  if(local) {
    Serial.println("LOCAL");
    Serial2.println("LOCAL");
  }
}

void MQTT_connect() {
 
  bool isconnected = false;
  // Stop if already connected.
  if(local) {
    isconnected = mqtt_local.connected();
  }
  else {
    isconnected = mqtt_remote.connected();
  }

  if(isconnected){
      return;
  }

  int8_t ret;
  ret = -1;
  while (ret !=0) {
    if(local) {
      ret = mqtt_local.connect();
    } else {
      ret = mqtt_remote.connect();
    }
    if(ret == 0){
      break;
    }

    Serial.println("CONNECTING");
    Serial2.println("CONNECTING");
    delay(1000);

    if(local){
      mqtt_local.disconnect();
    } else {
      mqtt_remote.disconnect();
    }
  }  
  Serial.println("CONNECTED");
}

String getValue(String data, char separator, int index)
{
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
              Serial.println("OK");
              Serial2.println("OK");
            } else {
              Serial.println("FAIL");
              Serial2.println("FAIL");
            }
          }
          break;
      }
    }
  }


void loop() {
  MQTT_connect();

  if(Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    Serial.println("SERIAL");
    send(line);
  }
  if(Serial2.available() > 0) {
    String line = Serial2.readStringUntil('\n');
    Serial.println("SERIAL2");
    send(line);
  }
}

