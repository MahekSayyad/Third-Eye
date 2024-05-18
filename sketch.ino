#define TINY_GSM_MODEM_SIM800  // Define the GSM modem model
#include <TinyGSM.h>
#include <GSMModem.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

// Define GSM and GPS modules
GSMModem modem(Serial1); // Adjust port for your setup
TinyGPS gps;

// Emergency button pin
const int emergencyButton = 2;

// Registered phone numbers
const String phoneNumber1 = "+1234567890";
const String phoneNumber2 = "+9876543210";

// Emergency message content base
const String messageBase = "Emergency! I need help. My location is:";

// Declare 'message' outside the loop for accessibility
String message;

void setup() {
  // Initialize serial communication, GSM module, and GPS module
  Serial.begin(9600);
  modem.restart();
  modem.waitForNetwork();

  // Start GPS communication
  gps.begin(Serial2); // Adjust port for your GPS module
  gps.setFallbacks(false); // Ensure accurate data

  // Attach interrupt to emergency button
  pinMode(emergencyButton, INPUT_PULLUP); // Assuming the button is connected to ground when pressed
  attachInterrupt(digitalPinToInterrupt(emergencyButton), emergencyTriggered, FALLING);
}

void loop() {
  // Main code for other smart blind stick functionalities
  // ...

  // Update GPS location periodically with delays
  static unsigned long lastGPSUpdate = 0;
  if (millis() - lastGPSUpdate >= 5000) { // Update every 5 seconds
    lastGPSUpdate = millis();

    if (gps.status() == TinyGPS::LOCATION_STATUS::FIX_OK) {
      // Get latitude and longitude
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();

      // Update message content with coordinates
      message = messageBase + " Lat: " + String(latitude, 6) + ", Long: " + String(longitude, 6);
    }
  }
}

void emergencyTriggered() {
  // Get latitude and longitude when the emergency button is pressed
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();

  // Send SMS to registered numbers with GPS coordinates
  for (const String& number : {phoneNumber1, phoneNumber2}) {
    modem.sendSMS(number, message);
    delay(1000); // Delay between messages
  }

  // Simulate popup message in Serial Monitor
  Serial.println("Emergency!");
  Serial.println("Help is on the way.");
  Serial.println("Location:");
  Serial.println("Lat: " + String(latitude, 6));
  Serial.println("Long: " + String(longitude, 6));

  // Optionally, add sound or vibration alerts
  // ...
}
