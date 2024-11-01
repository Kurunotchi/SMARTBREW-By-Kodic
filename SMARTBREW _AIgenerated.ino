#include <LiquidCrystal.h>
#include <Servo.h>
#include <NewPing.h>

/* Display Section */
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Button for Manual Selection */
const int Next = 3;   // Next Button
const int Select = 4; // Select Button

/* Variety of Coffee LCD Section */
const char* CoffeeVarieties[] = {"Black Coffee", "Chocolate", "Caramel"};
int selectedIndex = 0;

/* Ultrasonic Sensor Setup */
const int TRIG_PIN = 14;  // Trigger pin
const int ECHO_PIN = 15;  // Echo pin
const int MAX_DISTANCE = 200; // Maximum distance in cm
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

/* Color Sensor Setup */
const int COLOR_SENSOR_PIN_LEFT = A0;  // Left color sensor
const int COLOR_SENSOR_PIN_RIGHT = A1; // Right color sensor
const int COLOR_RED_THRESHOLD = 500;    // Threshold for detecting red (adjust as necessary)

/* Coffee Maker Mechanism Section */
Servo Cup;               // Wemos pin no.1 or D1 
Servo Black_Coffee;      // Wemos pin no.2 or D2
Servo Caramel_Coffee;    // Wemos pin no.3 or D5
Servo Chocolate;         // Wemos pin no.4 or D6

/* Relay Control Pins */
const int RELAY_FORWARD = 22;  // Relay pin for forward
const int RELAY_LEFT = 23;     // Relay pin for left turn
const int RELAY_RIGHT = 24;    // Relay pin for right turn
const int RELAY_STOP = 25;     // Relay pin to stop

/* Servo Positions */
const int BlackCoffeePosition = 90;     
const int CaramelCoffeePosition = 90;
const int CupPosition = 90;
const int ServoClosed = 0;

/* Setup Function */
void setup() {
    lcd.begin(16, 2);
    pinMode(Next, INPUT_PULLUP);
    pinMode(Select, INPUT_PULLUP);
    pinMode(RELAY_FORWARD, OUTPUT);
    pinMode(RELAY_LEFT, OUTPUT);
    pinMode(RELAY_RIGHT, OUTPUT);
    pinMode(RELAY_STOP, OUTPUT);
    displaySelection();

    Cup.attach(13);               // Cup dropper
    Black_Coffee.attach(2);       // Black coffee dropper
    Caramel_Coffee.attach(5);     // Caramel dropper
    Chocolate.attach(6);          // Chocolate dropper

    Serial.begin(9600);
}

/* Main Loop */
void loop() {
    // Check for obstacles
    unsigned int distance = sonar.ping_cm();
    if (distance > 0 && distance < 15) { // Adjust distance threshold
        stopRobot();
        return;
    }

    // If Select button is pressed, serve coffee
    if (digitalRead(Select) == LOW) {
        serveCoffee(selectedIndex);
        delay(300);
        followLineAndServe(); // Start line following after serving coffee
    }

    // If Next button is pressed
    if (digitalRead(Next) == LOW) {
        selectedIndex = (selectedIndex + 1) % 3; // Loop through options
        displaySelection();
        delay(300);
    }
}

/* Display Selection */
void displaySelection() {
    lcd.clear();
    lcd.print("Select ur Coffee: ");
    lcd.setCursor(0, 1);
    lcd.print(CoffeeVarieties[selectedIndex]);
}

/* Serve Coffee */
void serveCoffee(int index) {
    Cup.write(180);
    delay(400);
    Cup.write(0);
    delay(2000);

    switch (index) {
        case 0: // Black Coffee
            Black_Coffee.write(BlackCoffeePosition);
            delay(500);
            Black_Coffee.write(ServoClosed);
            break;
        case 1: // Caramel
            Caramel_Coffee.write(CaramelCoffeePosition);
            delay(500);
            Caramel_Coffee.write(ServoClosed);
            break;
        case 2: // Chocolate
            Chocolate.write(180);
            delay(300);
            Chocolate.write(0);
            delay(400);
            Chocolate.write(90);
            break;
    }
}

/* Follow Line and Serve */
void followLineAndServe() {
    while (true) {
        int leftSensorValue = analogRead(COLOR_SENSOR_PIN_LEFT);
        int rightSensorValue = analogRead(COLOR_SENSOR_PIN_RIGHT);

        if (leftSensorValue < COLOR_RED_THRESHOLD && rightSensorValue < COLOR_RED_THRESHOLD) {
            // Both sensors detect the line, move forward
            moveForward();
        } else if (leftSensorValue < COLOR_RED_THRESHOLD) {
            // Turn right
            turnRight();
        } else if (rightSensorValue < COLOR_RED_THRESHOLD) {
            // Turn left
            turnLeft();
        } else {
            // Stop if no line is detected
            stopRobot();
        }

        // Check for color red
        if (leftSensorValue >= COLOR_RED_THRESHOLD || rightSensorValue >= COLOR_RED_THRESHOLD) {
            // Stop and serve coffee
            stopRobot();
            serveCoffee(selectedIndex); // Serve coffee at the red color
            break; // Exit the loop after serving
        }
    }
}

/* Stop Robot */
void stopRobot() {
    digitalWrite(RELAY_STOP, HIGH);
    digitalWrite(RELAY_FORWARD, LOW);
    digitalWrite(RELAY_LEFT, LOW);
    digitalWrite(RELAY_RIGHT, LOW);
    Serial.println("Robot stopped.");
}

/* Movement Functions */
void moveForward() {
    digitalWrite(RELAY_FORWARD, HIGH);
    digitalWrite(RELAY_LEFT, LOW);
    digitalWrite(RELAY_RIGHT, LOW);
    digitalWrite(RELAY_STOP, LOW);
    Serial.println("Moving Forward");
}

void turnLeft() {
    digitalWrite(RELAY_LEFT, HIGH);
    digitalWrite(RELAY_FORWARD, LOW);
    digitalWrite(RELAY_RIGHT, LOW);
    digitalWrite(RELAY_STOP, LOW);
    Serial.println("Turning Left");
}

void turnRight() {
    digitalWrite(RELAY_RIGHT, HIGH);
    digitalWrite(RELAY_FORWARD, LOW);
    digitalWrite(RELAY_LEFT, LOW);
    digitalWrite(RELAY_STOP, LOW);
    Serial.println("Turning Right");
}
