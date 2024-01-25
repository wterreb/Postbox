#include <EEPROM.h>

#define relay 10
int solar_panel = A0;
int battery = A1;
int ldr = A2;
int pullup_ldr = 2;
int buttonPin = 3;
#define relay_on   LOW
#define relay_off  HIGH
#define correction_factor  1.05
int buttonState = 0;
int light_threshold = 800;
 int eeAddress = 0;

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(pullup_ldr, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Turn_5V_supply_off();
  light_threshold = (EEPROM.read(eeAddress) << 8) + EEPROM.read(eeAddress + 1);   // Get stored value out of EEPROM
}

void Turn_5V_supply_on()
{
   digitalWrite(relay, relay_on);
}

void Turn_5V_supply_off()
{
   digitalWrite(relay, relay_off); 
}


void loop() {
  digitalWrite(pullup_ldr, 1);  // Apply 5V to LDR so we can measure it
  float solarVoltage= analogRead(solar_panel) * (5.0 / 1023.0) * correction_factor;
  float batteryVoltage= analogRead(battery) * (5.0 / 1023.0) * correction_factor;
  int ldr_value= analogRead(ldr);
  digitalWrite(pullup_ldr, 0);  // Remove pullup to conserve power
  buttonState = digitalRead(buttonPin);

  Serial.print("solar = "); Serial.print(solarVoltage);Serial.print("  batt = ");Serial.print(batteryVoltage);Serial.print("  ldr = ");Serial.print(ldr_value); Serial.print("  pb = ");Serial.print(buttonState); Serial.print(" ");
  
  if (buttonState == 0) {
     // button pressed
     light_threshold = ldr_value - 10;  // the extra 10 is just for debounce
     EEPROM.write(eeAddress, light_threshold >> 8);
     EEPROM.write(eeAddress + 1, light_threshold & 0xFF);
  }

//  if ( ldr_value > light_threshold ) { 
  if ( (batteryVoltage > 3.9) && (ldr_value > light_threshold) ) {  
        Turn_5V_supply_on();
        Serial.print("*");
  }
 // if ( ldr_value < light_threshold ) {
  if ( (batteryVoltage < 3.70) || (ldr_value < light_threshold) ) {
    Turn_5V_supply_off();
    Serial.print("#");
  }
  Serial.println(" ");
  delay(1000);                   // wait for a second
}
