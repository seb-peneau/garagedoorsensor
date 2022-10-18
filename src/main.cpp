#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "main.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void disconnectWifi() {
  Serial.println("WiFi::disconnect");
  WiFi.disconnect(true);
  WiFi.forceSleepBegin();
  delay(1);
}

boolean connectToMqtt() {
  Serial.println("Mqtt::connect");
  String clientId = SENSOR_CLIENTID;
  boolean isConnected = mqttClient.connect(clientId.c_str(), mqttUser, mqttPassword);
  Serial.println(isConnected);
  return isConnected;
}

void disconnectMqtt() {
  Serial.println("Mqtt::disconnect");
  mqttClient.disconnect();
}

void onWifiConnected(const WiFiEventStationModeGotIP& event) {
  Serial.println("Mqtt::connection in progress");
  connectToMqtt();
}

void onWifiDisconnected(const WiFiEventStationModeDisconnected& event) {
  Serial.println("WiFi::is disconnected");
  disconnectMqtt();
}

void connectToWifi() {
  Serial.println("WiFi::connect");
  WiFi.forceSleepWake();
  delay(1000);
  WiFi.enableSTA(true);
  WiFi.onStationModeGotIP(onWifiConnected);
  WiFi.onStationModeDisconnected(onWifiDisconnected); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(WiFi.status());
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Sensor started");

  mqttClient.setServer(mqttServer, 1883);

  pinMode(REED_SWITCH_PIN, INPUT);
  pinMode(LED_BUILTIN,INPUT);//define LED as output interface

}

void loop() {
  Serial.println("---------------------Loop");
  if (doPublish == false) {
    Serial.println("Loop::nothing to publish");
    disconnectWifi();
  } else {
    Serial.println("Loop::Publication in progress");
    if (mqttClient.connected()) {
      Serial.println(mqttClient.state());
      Serial.println("Loop::Publication in progress::mqtt connected");
      boolean publicationSuccess = mqttClient.publish(mqttTopic, lastReedSwitchState == LOW ? "Closed" : "Opened");
      delay(1000);
      if (publicationSuccess == true) {
        Serial.println("Loop::Publication in progress::Publication succedeed");
        doPublish = false;
        disconnectWifi();
      } else {
        Serial.println("Loop::Publication in progress::Failed");
      }

    } else {
      Serial.println(mqttClient.state());
      Serial.println("Loop::Publication in progress::mqtt not ready");
      if (WiFi.status() == WL_CONNECTED) {
         Serial.println("Loop::Publication in progress::wifi is connected");
         connectToMqtt();
      } else {
        connectToWifi();
      }
    }
  }
  Serial.println("Loop::Read sensor value");
  int currentReedSwitchState = LOW;
  currentReedSwitchState = digitalRead(REED_SWITCH_PIN);

  if(currentReedSwitchState == HIGH)//When a signal is detected by magnetic ring sensor, LED will flash
  {
    Serial.println("Loop::Read sensor value::high");
  }
  else
  {
    Serial.println("Loop::Read sensor value::low");
  }

  if (currentReedSwitchState != lastReedSwitchState) {
    Serial.println("Loop::Read sensor value::value changed");
    lastReedSwitchState = currentReedSwitchState;
    connectToWifi();
    doPublish = true;
  } else {
    Serial.println("Loop::Read sensor value::value is the same");
    delay(20 * 1000);
  }
  
}
