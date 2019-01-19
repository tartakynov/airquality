#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

int sensor = 0;

void setup()
{
  lcd.init();
  lcd.backlight();
}

int aqi25(int pm25) {
  if (pm25 <= 12) {
    return pm25 * 50 / 12;
  } else if (pm25 <= 35) {
    return 50 + (pm25 - 12) * 50 / 23;
  } else if (pm25 <= 55) {
    return 100 + (pm25 - 35) * 5 / 2;
  } else if (pm25 <= 150) {
    return 150 + (pm25 - 55) * 50 / 95;
  } else if (pm25 <= 350) {
    return 50 + pm25;
  }

  return 400 + (pm25 - 350) * 2 / 3;
}

int aqi10(int pm10) {
  if (pm10 <= 55) {
      return pm10 * 50 / 55;
  } else if (pm10 <= 355) {
    return 50 + (pm10 - 55) / 2;
  } else if (pm10 <= 425) {
    return 200 + (pm10 - 355) * 10 / 7;
  } else if (pm10 <= 505) {
    return 300 + (pm10 - 425) * 10 / 8;
  }

  return pm10 - 105;
}

int aqi(int pm25, int pm10) {
  return max(aqi25(pm25), aqi10(pm10));
}

void loop()
{
  char line1[16], line2[16];
  int pm25 = sensor;
  int pm10 = max(0, sensor - 10);
  sprintf(line1, "AQI: %d", aqi(pm25, pm10));
  sprintf(line2, "PM 2.5: %d", sensor);
  lcd.setCursor(3,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  delay(1000);
  sensor++;
}
