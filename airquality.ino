#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int sensor = 0;

void setup()
{
  lcd.init();
  lcd.backlight();
}

int aqi(int pm25) {
  
}

void loop()
{
  char line1[16], line2[16];
  sprintf(line1, "AQI: %d", sensor);  
  sprintf(line2, "PM 2.5: %d", aqi(sensor));
  lcd.setCursor(3,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  delay(1000);
  sensor++;  
}
