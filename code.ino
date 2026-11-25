#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int S1 = 2; // Outer Sensor
const int S2 = 4; // Inner Sensor
int seats = 5;

int state = 0; // 0 = IDLE, 1 = Waiting for S2, 2 = Waiting for S1
unsigned long timer = 0;
const unsigned long TIMEOUT = 4000; // 2 seconds window

int last_s1 = LOW;
int last_s2 = LOW;

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Bus Occupancy   ");
  lcd.setCursor(0, 1);
  if (seats == 0) {
    lcd.print("   SEAT FULL    ");
  } else {
    char buf[17];
    sprintf(buf, "Free Seats: %-3d ", seats);
    lcd.print(buf);
  }
}

void setup() {
  lcd.init();                      
  lcd.backlight();
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  updateLCD();
}

void loop() {
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  unsigned long now = millis();

  // Reset state if person takes too long to cross both sensors
  if (state != 0 && (now - timer > TIMEOUT)) {
    state = 0;
  }

  switch (state) {
    case 0: // IDLE: Looking for the first sensor to be triggered
      if (s1 == HIGH && last_s1 == LOW) {
        state = 1; // S1 triggered first -> Entry underway (DO NOT update LCD yet)
        timer = now;
      } else if (s2 == HIGH && last_s2 == LOW) {
        state = 2; // S2 triggered first -> Exit underway (DO NOT update LCD yet)
        timer = now;
      }
      break;

    case 1: // Waiting for S2 to complete Entry
      if (s2 == HIGH && last_s2 == LOW) {
        if (seats > 0) seats--;
        updateLCD(); // <--- LCD updates ONLY AFTER full entry sequence!
        state = 0;   // Reset back to idle
      }
      break;

    case 2: // Waiting for S1 to complete Exit
      if (s1 == HIGH && last_s1 == LOW) {
        if (seats < 5) seats++;
        updateLCD(); // <--- LCD updates ONLY AFTER full exit sequence!
        state = 0;   // Reset back to idle
      }
      break;
  }

  last_s1 = s1;
  last_s2 = s2;
  delay(30);
}
