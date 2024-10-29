#include <LiquidCrystal.h>
#include <Servo.h>
#include <Stepper.h>

#define TRIG_PIN 4  // Pin for the ultrasonic trigger
#define ECHO_PIN 5  // Pin for the ultrasonic echo

/* SmartBrew: The Autonomous Robot Coffee Maker using IOT */

// Display setup
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Button setup
const int Next = 3;            // Next Button
const int Select = 4;          // Select Button

// Coffee varieties
const char* CoffeeVarieties[] = {"Black Coffee", "Caramel", "Chocolate"};
int selectedIndex = 0;

// Servo setup
Servo Cup;               // Cup dropper
Servo Black_Coffee;      // Black coffee dropper
Servo Caramel_Coffee;    // Caramel dropper
Servo Chocolate;         // Chocolate dropper

// Servo positions
const int BlackCoffeePosition = 90;     
const int CaramelCoffeePosition = 90;
const int CupOpenPosition = 180;
const int ServoClosed = 0;

// Stepper motor setup
const int stepsPerRevolution = 200;  // Adjust based on your stepper motor's specifications
int stepPin = 10;                    // Pin connected to A4988 STEP pin
int dirPin = 9;                      // Pin connected to A4988 DIR pin
Stepper myStepper(stepsPerRevolution, stepPin, dirPin);

// Ultrasonic sensor setup
long duration;
float distance;

void displaySelection(); // Function prototype
void displayOpeningScreen(); // Function prototype
float readUltrasonicDistance(); // Function prototype

void setup() {
    // Initialize LCD
    lcd.begin(16, 2);
    displayOpeningScreen(); // Show the opening screen

    // Initialize buttons
    pinMode(Next, INPUT_PULLUP);
    pinMode(Select, INPUT_PULLUP);

    // Attach servos
    Cup.attach(13);              
    Black_Coffee.attach(2);      
    Caramel_Coffee.attach(5);    
    Chocolate.attach(6);         

    // Initialize stepper motor pins
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    // Initialize ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {
    // Check for other button presses
    if (digitalRead(Next) == LOW) {
        selectedIndex = (selectedIndex + 1) % 3; // Loop through options
        displaySelection();
        delay(300); // Debounce delay
    }

    if (digitalRead(Select) == LOW) {
        serveCoffee(selectedIndex);
        delay(300); // Debounce delay
        
        // Indicate ready message and wait for button press to return to selection
        lcd.clear();
        lcd.print("Your coffee is");
        lcd.setCursor(0, 1);
        lcd.print("ready!");
        delay(2000); // Display for 2 seconds

        // Clear the LCD and display three coffee cups in the middle
        lcd.clear();
        lcd.setCursor(5, 1); // Set cursor for the first coffee cup
        lcd.write(byte(0));   // Display the first coffee cup
        lcd.setCursor(6, 1); // Set cursor for the second coffee cup
        lcd.write(byte(0));   // Display the second coffee cup
        lcd.setCursor(7, 1); // Set cursor for the third coffee cup
        lcd.write(byte(0));   // Display the third coffee cup

        // Wait for button press to return to selection
        while (true) {
            if (digitalRead(Next) == LOW || digitalRead(Select) == LOW) {
                displaySelection(); // Return to selection screen
                break;
            }
        }
    }
}

void displaySelection() {
    // Clear and display the selection
    lcd.clear();
    lcd.print("Select your Coffee:");
    lcd.setCursor(0, 1);
    lcd.print(CoffeeVarieties[selectedIndex]);
}

void displayOpeningScreen() {
    // Clear the LCD and display "KAPE"
    lcd.clear();
    lcd.setCursor(0, 0); // Start at the first row
    lcd.print("KAPE");
    delay(3000);          // Display for 3 seconds
    lcd.clear();          // Clear the screen after showing opening
}

void serveCoffee(int index) {
    // Dispense the cup
    Cup.write(CupOpenPosition);
    delay(400);
    Cup.write(ServoClosed);
    delay(2000); // Wait for the cup to be in place

    // Ultrasonic distance check
    distance = readUltrasonicDistance();
    if (distance < 35.0) {
        // Move stepper motor forward until distance is 25cm
        digitalWrite(dirPin, HIGH); // Set direction
        while (distance > 25.0) {
            for (int i = 0; i < stepsPerRevolution; i++) {
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(1000); // Step pulse duration
                digitalWrite(stepPin, LOW);
                delayMicroseconds(1000); // Step pulse duration
            }
            distance = readUltrasonicDistance(); // Update distance
        }

        // Stop the stepper motor at 25 cm
        lcd.clear();
        lcd.print("Dropping Coffee...");
        delay(2000); // Give some time before dropping coffee

        // Now pour the selected coffee
        pourCoffee(index);
        
        // Move stepper motor again until distance is 38cm
        digitalWrite(dirPin, HIGH); // Set direction
        while (true) {
            distance = readUltrasonicDistance(); // Update distance
            if (distance >= 38.0) {
                break; // Exit the loop when distance is 38cm or more
            }
            for (int i = 0; i < stepsPerRevolution; i++) {
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(1000); // Step pulse duration
                digitalWrite(stepPin, LOW);
                delayMicroseconds(1000); // Step pulse duration
            }
        }
        
        // Stop the stepper motor after reaching 38 cm
        lcd.clear();
        lcd.print("Cup Moved Back");
        delay(2000); // Display for 2 seconds
    }
}

void pourCoffee(int index) {
    switch (index) {
        case 0: // Black Coffee
            Black_Coffee.write(BlackCoffeePosition);
            delay(500); // Pouring of Black Coffee
            Black_Coffee.write(ServoClosed);
            break;
        case 1: // Caramel
            Caramel_Coffee.write(CaramelCoffeePosition);
            delay(500); // Pouring of Caramel
            Caramel_Coffee.write(ServoClosed);
            break;
        case 2: // Chocolate
            Chocolate.write(180);
            delay(300);
            Chocolate.write(0);
            delay(400);
            Chocolate.write(90); // Resetting 360-degree servo
            break;
    }
}

float readUltrasonicDistance() {
    // Send a pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Read the echo
    duration = pulseIn(ECHO_PIN, HIGH);
    
    // Calculate distance in cm
    return (duration * 0.034) / 2; // Speed of sound = 0.034 cm/µs
