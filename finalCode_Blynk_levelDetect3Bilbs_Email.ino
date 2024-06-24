#define BLYNK_TEMPLATE_ID "TMPL6IJCDjWzV"
#define BLYNK_TEMPLATE_NAME "My Bin"
#define BLYNK_AUTH_TOKEN "3VFHoRDHIZ2q76OPsQ1eoHu431mJvBs4"

#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

// Define pins for the ultrasonic sensor 
const int trigPin = 4;
const int echoPin = 17;

// Define pins for the LEDs

const int ledPin50 = 12;
const int ledPin75 = 14;
const int ledPin100 = 27;

// Define the bin height (in cm)
const int binHeight = 40; // Adjust based on your bin height

// WiFi credentials
const char* ssid = "SLT-4G-62F3";
const char* password = "828L8RQ3LF3";

// IFTTT webhook key
const char* iftttKey = "o9rVG7s_uhrtcLdClZHmfVqVt7OE_bE5ynk5WBK-Pag";
const char* iftttEventName = "BinFull"; // Define your event name

// Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(ledPin50, OUTPUT);
  pinMode(ledPin75, OUTPUT);
  pinMode(ledPin100, OUTPUT);

  // Turn off all LEDs at the start
  
  digitalWrite(ledPin50, LOW);
  digitalWrite(ledPin75, LOW);
  digitalWrite(ledPin100, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize Blynk
  Blynk.begin(auth, ssid, password,"blynk.cloud");
}

void loop() {
  // Measure the distance
  long duration = getDistance();
  int distance = duration * 0.034 / 2;

  // Calculate the fill level as a percentage
  int fillLevel = ((binHeight - distance) * 100) / binHeight;

  // Print the fill level for debugging
  Serial.print("Fill Level: ");
  Serial.print(fillLevel);
  Serial.println("%");

  // Update Blynk with the fill level
  Blynk.virtualWrite(V17, fillLevel); // Assume V0 is the virtual pin for the gauge

  // Control the LEDs based on the fill level
 

  if (fillLevel >= 50) {
    digitalWrite(ledPin50, HIGH);
    Blynk.virtualWrite(V12, 255); // Turn on Blynk LED widget on V2
  } else {
    digitalWrite(ledPin50, LOW);
    Blynk.virtualWrite(V12, 0); // Turn off Blynk LED widget on V2
  }

  if (fillLevel >= 75) {
    digitalWrite(ledPin75, HIGH);
    Blynk.virtualWrite(V14, 255); // Turn on Blynk LED widget on V3
  } else {
    digitalWrite(ledPin75, LOW);
    Blynk.virtualWrite(V14, 0); // Turn off Blynk LED widget on V3
  }

  if (fillLevel >= 90) {
    digitalWrite(ledPin100, HIGH);
    sendEmailNotification();
    Blynk.virtualWrite(V27, 255); // Turn on Blynk LED widget on V4
  } else {
    digitalWrite(ledPin100, LOW);
    Blynk.virtualWrite(V27, 0); // Turn off Blynk LED widget on V4
  }

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  } else {
    Serial.println("WiFi not connected");
  }

  // Run Blynk
  Blynk.run();

  // Wait before taking the next measurement
  delay(1000);
}

long getDistance() {
  // Send a pulse from the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo time
  return pulseIn(echoPin, HIGH);
}

void sendEmailNotification() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://maker.ifttt.com/trigger/" + String(iftttEventName) + "/json/with/key/" + String(iftttKey);
    
    // JSON payload
    String jsonPayload = "{\"this\":[{\"is\":{\"some\":[\"test\",\"data\"]}}]}";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(jsonPayload);
    
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Payload: " + payload);
      if (httpCode == 200) {
        Serial.println("Email notification sent successfully.");
      } else {
        Serial.println("Failed to send email notification. HTTP Code: " + String(httpCode));
      }
    } else {
      Serial.println("HTTP request failed.");
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}