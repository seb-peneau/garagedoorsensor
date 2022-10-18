#ifndef MAIN_H
#define MAIN_H

#define WIFI_SSID "YOURSSID"
#define WIFI_PASSWORD "YOUPASSWORD"

#define SENSOR_CLIENTID "SENSORCLIENTID"
const char* mqttServer = "MQTTSERVERADDRESS"; 
const char* mqttUser = "MQTTUSER";
const char* mqttPassword = "MQTTPASSWORD";
const char* mqttTopic = "MQTTTOPIC";

#define REED_SWITCH_PIN YOURPINNUMBER

int lastReedSwitchState = LOW;
boolean doPublish = false;

#endif
