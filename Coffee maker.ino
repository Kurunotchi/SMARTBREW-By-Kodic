#include <LiquidCrystal.h>
#include <Servo.h>
#include <Stepper.h>

#define TRIG_PIN 4  // Ultrasonic trigger pin
#define ECHO_PIN 5  // Ultrasonic echo pin

// LCD setup
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Button setup
const int BUTTON_NEXT = 3;      // Next button
const int BUTTON_SELECT = 2;    // Select button

// Coffee varieties
const char* coffeeVarieties[] = {"Black Coffee", "Caramel", "Chocolate"};
int selectedIndex = 0;

// Servo setup
Servo cupServo;               
Servo blackCoffeeServo;      
Servo caramelCoffeeServo;    
Servo chocolateServo;         

// Servo positions
const int BLACK_COFFEE_POS = 90;     
const int CARAMEL_COFFEE_POS = 90;
const int CUP_OPEN_POS = 180;
const int SERVO_CLOSED = 0;

// Stepper motor setup
const int stepsPerRevolution = 200;  
int stepPin = 10;                    
int dirPin = 9;                      
Stepper stepper(stepsPerRevolution, stepPin, dirPin);

// Ultrasonic sensor variables
long duration;
float distance;

// Function declarations
void displaySelection();
void displayOpeningScreen();
void serveCoffee(int index);
void pourCoffee(int index);
float readUltrasonicDistance();

void setup() {
    // Initialize LCD
    lcd.begin(16, 2);
    displayOpeningScreen(); 

    // Initialize buttons
    pinMode(BUTTON_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_SELECT, INPUT_PULLUP);

    // Attach servos
    cupServo.attach(13);              
    blackCoffeeServo.attach(3);      
    caramelCoffeeServo.attach(4);    
    chocolateServo.attach(5);         

    // Initialize stepper motor pins
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    // Initialize ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {
    // Handle button presses
    if (digitalRead(BUTTON_NEXT) == LOW) {
        selectedIndex = (selectedIndex + 1) % 3; // Cycle through options
        displaySelection();
        delay(300); // Debounce delay
    }

    if (digitalRead(BUTTON_SELECT) == LOW) {
        serveCoffee(selectedIndex);
        delay(300); // Debounce delay
        lcd.clear();
        lcd.print("Your coffee is");
        lcd.setCursor(0, 1);
        lcd.print("ready!");
        delay(2000); 

        // Return to selection screen
        while (true) {
            if (digitalRead(BUTTON_NEXT) == LOW || digitalRead(BUTTON_SELECT) == LOW) {
                displaySelection(); 
                break;
            }
        }
    }
}

void displaySelection() {
    lcd.clear();
    lcd.print("Select your Coffee:");
    lcd.setCursor(0, 1);
    lcd.print(coffeeVarieties[selectedIndex]);
}

void displayOpeningScreen() {
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("KAPE");
    delay(3000);         
    lcd.clear();         
}

void serveCoffee(int index) {
    cupServo.write(CUP_OPEN_POS);
    delay(400);
    cupServo.write(SERVO_CLOSED);
    delay(2000); 

    distance = readUltrasonicDistance();
    if (distance < 35.0) {
        digitalWrite(dirPin, HIGH); 
        while (distance > 25.0) {
            for (int i = 0; i < stepsPerRevolution; i++) {
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(1000);
                digitalWrite(stepPin, LOW);
                delayMicroseconds(1000);
            }
            distance = readUltrasonicDistance(); 
        }

        lcd.clear();
        lcd.print("Dropping Coffee...");
        delay(2000); 

        pourCoffee(index);
        
        digitalWrite(dirPin, HIGH); 
        while (true) {
            distance = readUltrasonicDistance(); 
            if (distance >= 38.0) {
                break; 
            }
            for (int i = 0; i < stepsPerRevolution; i++) {
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(1000);
                digitalWrite(stepPin, LOW);
                delayMicroseconds(1000);
            }
        }
        
        lcd.clear();
        lcd.print("Cup Moved Back");
        delay(2000); 
    }
}

void pourCoffee(int index) {
    switch (index) {
        case 0:
            blackCoffeeServo.write(BLACK_COFFEE_POS);
            delay(500); 
            blackCoffeeServo.write(SERVO_CLOSED);
            break;
        case 1:
            caramelCoffeeServo.write(CARAMEL_COFFEE_POS);
            delay(500); 
            caramelCoffeeServo.write(SERVO_CLOSED);
            break;
        case 2:
            chocolateServo.write(180);
            delay(300);
            chocolateServo.write(0);
            delay(400);
            chocolateServo.write(90); 
            break;
    }
}

float readUltrasonicDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    
    return (duration * 0.034) / 2; 
}
