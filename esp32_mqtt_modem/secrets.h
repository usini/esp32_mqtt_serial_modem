#define NAME                   "gateway1"
#define TOPIC_LIMIT            16

/* SERIAL PORT */
//M5Stick C
#define RXD2                   26
#define TXD2                   36

/* LOCAL MQTT SERVER */
#define LOCAL_SSID             "" // Wifi Name
#define LOCAL_PASS             "" // Wifi Password
#define LOCAL_MQTT_SERVER      "" //IP Address
#define LOCAL_MQTT_SERVERPORT  1883
#define LOCAL_MQTT_USERNAME    ""
#define LOCAL_MQTT_KEY         ""

/* REMOTE MQTT SERVER */ 
#define REMOTE_SSID            ""
#define REMOTE_PASS            ""
#define REMOTE_MQTT_SERVER     ""
#define REMOTE_MQTT_SERVERPORT  8883                   // use 8883 for SSL
#define REMOTE_MQTT_USERNAME   ""
#define REMOTE_MQTT_KEY        ""

/* REMOTE MQTT SERVER */

// Get your certificate here : https://projects.petrucci.ch/esp32/?page=ssl.php
// Use DST Root CA X3 (not your certificate)
const char* rootCACertificate = "";


