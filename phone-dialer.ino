#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dial pin goes HIGH while the dialer is turned; is normally low
const int dialPin = 13; // white/brown wires from terminal

// Rotation pin goes LOW shortly for each number when releasing the dialer
const int rotationPin = 12; // green/yellow wires from terminal

// The delay for debouncing buttons
const int debounceDelay = 2; // ms

// Tracking variables for debouncing
int currentStateDial = LOW; // current confirmed state
int lastStateDial = LOW; // tracking state
int lastChangeDial = 0; // last time tracking state changed
int currentStateRotation = HIGH; // current confirmed state
int lastStateRotation = HIGH; // tracking state
int lastChangeRotation = 0; // last time tracking state changed

int digitTicks = 0;
unsigned long currentNumber = 0;

// LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(dialPin, INPUT);
  pinMode(rotationPin, INPUT);
  
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
}

void loop() {
  // First we handle the dial pin
  
  int stateDial = digitalRead(dialPin);
  if (stateDial != lastStateDial) {
    lastChangeDial = millis();
    lastStateDial = stateDial;
  }
    
  if ((millis() - lastChangeDial) > debounceDelay && currentStateDial != stateDial) {
    currentStateDial = stateDial;
    if (stateDial == LOW)
      Serial.println("stateDial changed state from HIGH to LOW");
    else
      Serial.println("stateDial changed state from LOW to HIGH");
    
    // Restart the tick count when we notice the user turns the dial
    if (stateDial == HIGH)
      digitTicks = 0;
    
    // When the rotator returns to its home position, stateDial goes LOW
    if (stateDial == LOW && digitTicks > 0) {
      // digitTicks is basically the number, though 10 ticks should be 0
      // so we use the remainder of a division by 10
      int dialedDigit = digitTicks % 10;
      // shift the previous digit up one position by multiplying by 10
      currentNumber = (currentNumber * 10) + dialedDigit;
      
      Serial.print("Current number: ");
      Serial.println(currentNumber);
      
      lcd.print(dialedDigit);
    }
  }
  
  // Now we handle the rotation pin
  
  int stateRotation = digitalRead(rotationPin);
  if (stateRotation != lastStateRotation) {
    lastChangeRotation = millis();
    lastStateRotation = stateRotation;
  }

  if ((millis() - lastChangeRotation) > debounceDelay && currentStateRotation != stateRotation) {
    currentStateRotation = stateRotation;
    if (stateRotation == LOW)
      Serial.println("stateRotation changed state from HIGH to LOW");
    else
      Serial.println("stateRotation changed state from LOW to HIGH");
    
    // Count the ticks for the current digit when the state goes from LOW back to HIGH
    if (stateRotation == HIGH && currentStateDial == HIGH)
      digitTicks++;
  }

}

