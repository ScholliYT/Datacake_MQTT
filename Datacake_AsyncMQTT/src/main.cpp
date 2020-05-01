#if SSL_ENABLED
#include <WiFiClientSecure.h>
#else
#include <WiFi.h>
#endif
#include <PubSubClient.h>

#include "secrets.h"

#define BUTTON_PIN 34
#define LED_R_PIN 33
#define LED_G_PIN 32
#define LED_B_PIN 35

bool buttonLastState = false;

#if SSL_ENABLED
WiFiClientSecure wifiClient;
#else
WiFiClient wifiClient;
#endif

PubSubClient mqttClient(wifiClient);

void wifiConnect() {
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to WiFi...");

    uint8_t retryCounter = 10;
    while (WiFi.status() != WL_CONNECTED && retryCounter > 0) {
        Serial.print(".");
        delay(500);
        retryCounter--;
    }

    if (retryCounter > 0) {
        Serial.println("connected");
    } else {
        Serial.print("failed, status code =");
        Serial.println(WiFi.status());
        Serial.println("Restarting in 5 sec");
        delay(5000);
        ESP.restart();
    }
}

void mqttConnect() {
    while (!mqttClient.connected()) {
        Serial.print("MQTT connecting...");
        // MQTT Hostname should be empty for Datacake
        if (mqttClient.connect("", MQTT_USER, MQTT_PASS)) {
            Serial.println("connected");
            mqttClient.subscribe(MQTT_SUB_TOPIC);
        } else {
            Serial.print("failed, status code =");
            Serial.print(mqttClient.state());
            Serial.println("try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("MQTT Received [%s]: ", topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.printf(" (len=%d)", length);
    Serial.println();

    if(memcmp(payload, "true", min(length, (unsigned int)4)) == 0) {
        digitalWrite(LED_R_PIN, LOW);
        digitalWrite(LED_G_PIN, HIGH);
    } else {
        digitalWrite(LED_R_PIN, HIGH);
        digitalWrite(LED_G_PIN, LOW);
    }
}

void setup() {
    delay(2000);
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT);
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);

    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_AP_STA);
    wifiConnect();

    #if SSL_ENABLED
    wifiClient.setCACert(root_ca);  // Verify CA certificate
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    #else
    mqttClient.setServer(MQTT_HOST, MQTT_PORT_INSECURE);
    #endif
    
    mqttClient.setCallback(mqttCallback);
    mqttConnect();
}

unsigned long lastloop = 1;
void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnect();
    }

    if (!mqttClient.connected()) {
        mqttConnect();
    }

    mqttClient.loop();

    // TODO: Unbounce button
    bool button = digitalRead(BUTTON_PIN);
    if (button != buttonLastState) {
        buttonLastState = button;
        mqttClient.publish(MQTT_PUB_TOPIC, button ? "1" : "0");
        Serial.print("Button is now: ");
        Serial.println(button ? "true" : "false");
    }
}