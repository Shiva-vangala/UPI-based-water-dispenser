#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// SIM900L connections
#define SIM900L_TX_PIN 7
#define SIM900L_RX_PIN 8

SoftwareSerial sim900l(SIM900L_TX_PIN, SIM900L_RX_PIN);
int amt = 0;
int litre_per_rupee = 1;
int time_per_litre = 10;

int motor = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the LCD address if necessary

// LCD backlight control pins
int backlightPin = 10;
int backlightBrightness = 255; // Adjust the backlight brightness (0-255)

void setup() {
  Serial.begin(9600);
  sim900l.begin(9600);
  pinMode(motor, OUTPUT);
  digitalWrite(motor, HIGH);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("SIM900L init.");

  // Initialize backlight pin
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, backlightBrightness); // Set backlight brightness
  
  // Initialize SIM900L
  delay(2000);
  sim900l.println("AT"); // AT test command
  updateSerial();
  delay(1000);
  sim900l.println("AT+CMGF=1");  // Set SMS mode to text
  updateSerial();
  delay(1000);
  sim900l.println("AT+CNMI=1,2,0,0,0");  // Enable SMS notifications
  updateSerial();
  delay(1000);

  lcd.clear();
  lcd.print("SIM900L init.");
  lcd.setCursor(0, 1);
  lcd.print("Ready.");
}

void loop() {
  if (sim900l.available()) {
    String message = sim900l.readString();
    Serial.println(message);
    
    if (message.indexOf("credited") != -1) {
      int startIndex = message.indexOf("Rs") + 3;
      int endIndex = message.indexOf(" on");
      String amount = message.substring(startIndex, endIndex);
      amt = amount.toInt();
      Serial.print("Amt received: ");
      Serial.println(amt);

      lcd.clear();
      lcd.print("Amount received:");
      lcd.setCursor(0, 1);
      lcd.print(amount);
      
      water_dispense();
    } else if (message.indexOf("debited") != -1) {
      int startIndex = message.indexOf("INR") + 4;
      int endIndex = message.indexOf(".00");
      String amount = message.substring(startIndex, endIndex);
      amt = amount.toInt();
      Serial.print("Amt debited: ");
      Serial.println(amt);

      lcd.clear();
      lcd.print("Amount debited:");
      lcd.setCursor(0, 1);
      lcd.print(amount);
      
      // Handle debited amount, if necessary
    }
  }
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim900l.write(Serial.read());
  }
  while (sim900l.available()) {
    Serial.write(sim900l.read());
  }
}

void water_dispense() {
  Serial.println("Water ready to dispense");
  digitalWrite(motor, LOW);
  for (int i = time_per_litre * amt; i > 0; i--) {
    delay(100);
    Serial.println(i);
  }
  digitalWrite(motor, HIGH);
  Serial.println("Water dispensed.");

  lcd.clear();
  lcd.print("Dispensed : ");
  lcd.setCursor(0, 1);
  lcd.print(amt);
  lcd.print(" required water");
}
