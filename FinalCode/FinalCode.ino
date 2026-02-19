//--------------------------- 👾 TEAM COFFEE LAKE 👾 -------------------------------
//         ──────────────────────────────── 🌐 ────────────────────────────────
//
//           Created by:
//           - ANIRUDDHA DEY  BTECH/60115/24
//           - YASH RAJ VARDHAN BTECH/60129/24
//           - SAVYASACHI SAWARN BTECH/60138/24
//
//-----------------------------------------------------------------------------------

#include <Servo.h> //initialising library

// Defining pins for motors, ultrasonic sensor, and servo
#define In1 15      // Motor IN1 connected to D8
#define In2 13      // Motor IN2 connected to D7
#define In3 2       // Motor IN3 connected to D4
#define In4 0       // Motor IN4 connected to D3
#define TrigPin 5   // Ultrasonic sensor trigger connected to D1
#define EchoPin 4   // Ultrasonic sensor echo connected to D2
#define servoPin 16 // Servo motor connected to D0

Servo myservo; // naming the servo motor

void setup()
{
    Serial.begin(9600); // Initialising the serial monitor
    Serial.println("Starting the bot...");

    // Set pin modes
    pinMode(In1, OUTPUT);
    pinMode(In2, OUTPUT);
    pinMode(In3, OUTPUT);
    pinMode(In4, OUTPUT);
    pinMode(TrigPin, OUTPUT);
    pinMode(EchoPin, INPUT); // set echo pin as input

    // Initialize the servo
    myservo.attach(servoPin); // assigning pin to the nodemcu
    myservo.write(60);        // Initial position x=60 degrees
    delay(2000);              // Allow servo to move
    Serial.println("Started.");
}

void loop()
{
    long duration, distance;
    // Send pulse to trigger ultrasonic sensor
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);

    // Read the duration of the echo
    duration = pulseIn(EchoPin, HIGH); // Time taken for Pulse to Calculate

    // Calculate distance
    distance = (duration / 2) / 29.1;
    Serial.print("Distance is CM = ");
    Serial.println(distance);

    if (distance <= 10)
    {
        Serial.println("Coffee detected, executing avoidance maneuvers.");
        Stop();
        delay(200);

        // Check for right side only to minimize turns
        lookRight();
        delay(500); // Allow time for the servo to reach the position

        // Check distance after looking right
        long rightDistance = getDistance();

        if (rightDistance > 15)
        {
            Serial.println("Space on the right, turning right.");
            TurnRight(); // Correctly turning right
            delay(1000);
        }
        else
        {
            Serial.println("No space to turn right, turning left instead.");
            TurnLeft(); // Call TurnLeft if no space to turn right
            delay(1000);
        }

        // Hold position for 500 ms after turning
        Stop();
        delay(500); // Hold for 500 ms

        // Check forward space after turning
        long forwardDistance = getDistance();
        if (forwardDistance > 5)
        {
            Serial.println("Space forward is clear, moving forward.");
            Forward();
        }
        else
        {
            Serial.println("Space forward is blocked, stopping.");
            Stop();
        }

        delay(500);
    }
    else
    {
        Serial.println("No Coffee detected, moving forward.");
        Forward();
    }
}

void lookRight()
{
    Serial.println("Looking right...");
    myservo.write(180); // Max right position
    delay(1500);
    myservo.write(60); // Return to center
    delay(1000);
}

/*void lookLeft() // Not required as turning left will occur if right is blocked
{
  Serial.println("Looking left...");
  serOvO.write(-180); // Left direction max
  delay(1500);
  serOvO.write(60); // Returns to center
  delay(1000);
}*/

long getDistance()
{
    // Send pulse to the ultrasonic sensor
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);

    // Read the duration of the echo
    long duration = pulseIn(EchoPin, HIGH);

    // Calculate and return distance
    return (duration / 2) / 29.1; // Converting distance in cm
}

void Stop()
{
    Serial.println("Stopping motors...");
    digitalWrite(In1, LOW);
    digitalWrite(In2, LOW);
    digitalWrite(In3, LOW);
    digitalWrite(In4, LOW);
}

void Forward()
{
    Serial.println("Moving forward...");
    digitalWrite(In1, LOW);
    digitalWrite(In2, HIGH);
    digitalWrite(In3, LOW);
    digitalWrite(In4, HIGH);
}

/*
void moveBackward() // Not used as reversing in a maze isn't required
{
  Serial.println("Moving Backward...");
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
}
*/

void TurnRight()
{
    Serial.println("Turning right...");
    digitalWrite(In1, HIGH);
    digitalWrite(In2, LOW);
    digitalWrite(In3, LOW);
    digitalWrite(In4, HIGH);
}

void TurnLeft()
{
    Serial.println("Turning left...");
    digitalWrite(In1, LOW);
    digitalWrite(In2, HIGH);
    digitalWrite(In3, HIGH);
    digitalWrite(In4, LOW);
}