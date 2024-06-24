#include <Servo.h>

// Define pins for the ultrasonic sensor
const int trigPin = 2;
const int echoPin = 3;

// Define pin for the LED
const int ledPin = 13;

// Define pin for the servo motor
const int servoPin = 9;

// Define the maximum distance to trigger the LED and servo (in cm)
const int triggerDistance = 50;
  
// Define a minimum distance to avoid false readings
const int minDistance = 2; // Adjust this value as needed

// Create a Servo object
Servo myServo;

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Initialize the pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Attach the servo motor to the servo pin
  myServo.attach(servoPin);

  // Move the servo to the initial position (closed lid)
  myServo.write(0); // 0 degrees
}

void loop() {
  // Send a pulse from the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo time
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  int distance = duration * 0.034 / 2;

  // Print the distance for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if the distance is valid and less than the trigger distance
  if (distance >= minDistance && distance <= triggerDistance) {
    // Turn the LED on
    digitalWrite(ledPin, HIGH);

    // Open the lid by moving the servo to 90 degrees
    myServo.write(180); // 90 degrees (open lid)
  } else {
    // Turn the LED off
    digitalWrite(ledPin, LOW);

    // Close the lid by moving the servo back to 0 degrees
    myServo.write(0); // 0 degrees (closed lid)
  }

  // Wait before taking the next measurement
  delay(1000);
}
