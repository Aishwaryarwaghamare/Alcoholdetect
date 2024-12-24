#include <LiquidCrystal.h>

// Pin Definitions
#define MQ3_PIN A0            // Analog pin connected to MQ-3 sensor
#define BUZZER_PIN 8          // Digital pin connected to the buzzer
#define LED_PIN 13            // Digital pin connected to the LED
#define BUTTON_PIN 7          // Digital pin connected to a button
#define THRESHOLD_DEFAULT 400 // Default threshold for alcohol detection

// LCD Pin Configuration
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Global Variables
int threshold = THRESHOLD_DEFAULT;         // Threshold for alcohol detection
volatile bool calibrateMode = false;       // Flag for calibration mode
unsigned long lastUpdateTime = 0;          // For display refresh
const unsigned long refreshInterval = 200; // LCD refresh rate in milliseconds

void setup()
{
    // Initialize pins
    pinMode(MQ3_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with internal pull-up resistor

    // Initialize Serial Communication
    Serial.begin(9600);

    // Initialize LCD
    lcd.begin(16, 2);
    displayWelcomeMessage();
    delay(2000);
    lcd.clear();

    // Attach interrupt for button press
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), toggleCalibrationMode, FALLING);
}

void loop()
{
    // Enter calibration mode if triggered
    if (calibrateMode)
    {
        enterCalibrationMode();
    }

    // Read the sensor value
    int sensorValue = analogRead(MQ3_PIN);

    // Check if alcohol level exceeds the threshold
    if (sensorValue > threshold)
    {
        triggerAlert(sensorValue);
    }
    else
    {
        resetAlert(sensorValue);
    }

    // Update the LCD display periodically
    if (millis() - lastUpdateTime >= refreshInterval)
    {
        updateDisplay(sensorValue);
        lastUpdateTime = millis();
    }

    // Log data to the Serial Monitor
    logData(sensorValue);
}

// Displays a welcome message on the LCD
void displayWelcomeMessage()
{
    lcd.setCursor(0, 0);
    lcd.print("Alcohol System");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
}

// Updates the LCD with sensor value and status
void updateDisplay(int sensorValue)
{
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(sensorValue);
    lcd.print("   "); // Clear old digits

    lcd.setCursor(0, 1);
    if (sensorValue > threshold)
    {
        lcd.print("Status: ALERT  ");
    }
    else
    {
        lcd.print("Status: Normal ");
    }
}

// Triggers the alert by activating the buzzer and LED
void triggerAlert(int sensorValue)
{
    digitalWrite(LED_PIN, HIGH);    // Turn on LED
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
    Serial.println("ALERT: Alcohol Detected!");
}

// Resets the alert by deactivating the buzzer and LED
void resetAlert(int sensorValue)
{
    digitalWrite(LED_PIN, LOW);    // Turn off LED
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
}

// Enters calibration mode to set a new threshold
void enterCalibrationMode()
{
    calibrateMode = false; // Reset calibration flag
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibration Mode");
    lcd.setCursor(0, 1);
    lcd.print("Reading...");

    int sum = 0;
    for (int i = 0; i < 10; i++)
    { // Take 10 readings
        sum += analogRead(MQ3_PIN);
        delay(100); // Short delay for accurate reading
    }
    threshold = sum / 10 + 50; // Set new threshold
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New Threshold:");
    lcd.setCursor(0, 1);
    lcd.print(threshold);
    delay(2000);
    lcd.clear();
}

// Toggles the calibration mode using an interrupt
void toggleCalibrationMode()
{
    calibrateMode = true;
}

// Logs sensor data and status to the Serial Monitor
void logData(int sensorValue)
{
    Serial.print("Sensor Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Threshold: ");
    Serial.print(threshold);
    if (sensorValue > threshold)
    {
        Serial.println(" | Status: ALERT");
    }
    else
    {
        Serial.println(" | Status: Normal");
    }
}
