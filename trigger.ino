#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// LCD Pannel buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int sound = 0;
int soundmax = 0;
int sens = 0; //sensor treshold
int lags = 0; //trigger delay
int menu = 1;
char numer[5];


void setup() {
  Serial.begin(9600);  //debug serial connection
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  //Flash trigger pin
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  //Read settings from EEPROM
  EEPROM.get(0, sens);
  if (sens > 1000 || sens < 0) sens = 500;
  EEPROM.get(10, lags);
}

void loop() {
  digitalWrite(2, LOW);
  lcd_key = read_LCD_buttons();
  
  //button actions
  switch (lcd_key) {
    case btnRIGHT: {
        if (menu < 4) menu++; else menu = 1;
        break;
      }
    case btnLEFT: {
        if (menu > 1) menu--; else menu = 4;
        break;
      }
    case btnUP: {
        if (menu == 1) sens = sens + 10;
        if (menu == 2) lags = lags + 1;
        break;
      }
    case btnDOWN: {
        if (menu == 1) sens = sens - 10;
        if (menu == 2) lags = lags - 1;
        break;
      }
    case btnSELECT: {
        if (menu == 3) stat();
        if (menu == 4) wait(sens, lags);
        break;
      }
    case btnNONE: {
        break;
      }
  }

  //settings roll over
  if (sens > 1000) sens = 0;
  if (sens < 0) sens = 1000;
  if (lags < 0) lags = 0;

  //draw screen
  lcd.setCursor(0, 0);
  lcd.print("sens delay");
  lcd.setCursor(12, 0);
  lcd.print("?");
  lcd.setCursor(15, 0);
  lcd.print(">");

  lcd.setCursor(1, 1);
  sprintf(numer, "%4d", sens);
  lcd.print(numer);
  lcd.setCursor(7, 1);
  sprintf(numer, "%4d", lags);
  lcd.print(numer);

  //draw selected option on screen
  lcd.setCursor(0, 1);
  if (menu == 1) lcd.print("*"); else lcd.print(" ");
  lcd.setCursor(6, 1);
  if (menu == 2) lcd.print("*"); else lcd.print(" ");
  lcd.setCursor(11, 0);
  if (menu == 3) lcd.print("*"); else lcd.print(" ");
  lcd.setCursor(14, 0);
  if (menu == 4) lcd.print("*"); else lcd.print(" ");

  //settings change delay for buttons
  if (lcd_key != btnNONE) delay(100);
  if (lcd_key == btnRIGHT || lcd_key == btnLEFT) delay(300);
}

//Read resistior ladder button
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;
}

// Trigger Flash
void flash(int lag) {
  lcd.clear();
  lcd.write("Wyzwolenie...");
  delay (lag);
  digitalWrite(2, HIGH);
  delay (1000);
  digitalWrite(2, LOW);
}

//wait for sound
void wait(int s, int l) {
  lcd.clear();
  lcd.write("Kalibracja...");
  EEPROM.put(0, s);
  EEPROM.put(10, l);
  delay(1000);
  lcd.clear();
  lcd.write("Czekam na dźwięk");
  lcd.setCursor(4, 1);
  lcd.write("<wstecz");
  while (lcd_key != btnLEFT) {
    sound = analogRead(1);
    if (sound >= s) {
      flash(l);
      break;
    }
    lcd_key = read_LCD_buttons();
  }
  lcd.clear();
}

//check current and max sound levels
void stat() {
  lcd.clear();
  lcd.write("<wstecz   >reset");
  while (lcd_key != btnLEFT) {
    if (lcd_key == btnRIGHT) soundmax = 0;
    sound = analogRead(1);
    if (sound > soundmax)
    {
      soundmax = sound;
    }
    lcd.setCursor(1, 1);
    sprintf(numer, "%4d", sound);
    lcd.print(numer);
    lcd.setCursor(7, 1);
    sprintf(numer, "%4d", soundmax);
    lcd.print(numer);
    lcd_key = read_LCD_buttons();
  }
  lcd.clear();
}
  
