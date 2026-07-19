#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker details
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "servo/control";
const char* clientID = "ESP32_Servo_Controller";

// Twilio credentials
const char* twilio_account_sid = "YOUR_TWILIO_ACCOUNT_SID";
const char* twilio_auth_token = "YOUR_TWILIO_AUTH_TOKEN";
const char* twilio_from_number = "YOUR_TWILIO_NUMBER";
const char* mom_phone_number = "CAREGIVER_PHONE_NUMBER";


// WiFi and MQTT
WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo;

// Sensors
const int soundSensorPin = 32;
const int moistureSensorPin = 35;

const int soundThreshold = 1200;
const int moistureThreshold = 3000;

bool smsSentSound = false;
bool smsSentMoisture = false;
bool motorRunning = false;

// Timing
unsigned long lastSensorRead = 0;
const unsigned long readInterval = 1000; // 1 second

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Servo and sensor setup
  myServo.attach(15);
  pinMode(soundSensorPin, INPUT);
  pinMode(moistureSensorPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorRead >= readInterval) {
    lastSensorRead = currentMillis;

    int soundLevel = analogRead(soundSensorPin);
    int moistureLevel = analogRead(moistureSensorPin);

    Serial.print("Sound Level: ");
    Serial.print(soundLevel);
    Serial.print(" | Moisture Level: ");
    Serial.println(moistureLevel);

    // Sound condition
    if (soundLevel > soundThreshold) {
      if (!smsSentSound) {
        sendSMSToMom("Baby is crying! Please check immediately.");
        smsSentSound = true;
      }
    } else {
      smsSentSound = false;  // reset when baby stops crying
    }

    // Moisture condition
    if (moistureLevel < moistureThreshold) {
      if (!smsSentMoisture) {
        sendSMSToMom("Baby's sheets are wet! Please take action.");
        smsSentMoisture = true;
      }
    } else {
      smsSentMoisture = false;  // reset when sheets dry
    }
  }

  if (motorRunning) {
    myServo.write(0);
    delay(500);
    myServo.write(180);
    delay(500);
  } else {
    myServo.write(90);
    delay(100);
  }
}

// MQTT reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// MQTT message handler
void callback(char* topic, byte* payload, unsigned int length) {
  String incomingMessage;
  for (unsigned int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }
  incomingMessage.toUpperCase();
  Serial.print("MQTT message received: ");
  Serial.println(incomingMessage);

  if (incomingMessage == "ON") {
    motorRunning = true;
    Serial.println("Mom started servo via MQTT");
  } else if (incomingMessage == "OFF") {
    motorRunning = false;
    Serial.println("Mom stopped servo via MQTT");
  }
}

// Send SMS using Twilio
void sendSMSToMom(String messageBody) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.twilio.com/2010-04-01/Accounts/";
    url += twilio_account_sid;
    url += "/Messages.json";

    String auth = twilio_account_sid;
    auth += ":";
    auth += twilio_auth_token;

    http.begin(url);
    http.addHeader("Authorization", "Basic " + base64Encode(auth));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "From=" + String(twilio_from_number) + "&To=" + String(mom_phone_number) + "&Body=" + messageBody;

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      Serial.println("SMS Sent Successfully");
      Serial.println(http.getString());
    } else {
      Serial.print("Error Sending SMS: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send SMS.");
  }
}

// Base64 encoding
String base64Encode(String input) {
  const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String output = "";
  int val = 0, valb = -6;
  for (unsigned char c : input) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      output += base64_table[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }
  if (valb > -6) output += base64_table[((val << 8) >> (valb + 8)) & 0x3F];
  while (output.length() % 4) output += '=';
  return output;
}
