#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

int pm25 = 0, pm10 = 0, aqi = 0;
char line[16], serial[64];

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

int getAQI25(int pm25) {
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

int getAQI10(int pm10) {
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

int getAQI(int pm25, int pm10) {
  return max(getAQI25(pm25), getAQI10(pm10));
}

void loop()
{
  pm25++; // fake sensor input
  pm10 = max(0, pm25 - 10); // fake sensor input
  aqi = getAQI(pm25, pm10);

  // print air quality index on the first line
  sprintf(line, "AQI: %d", aqi);
  lcd.setCursor(3,0);
  lcd.print(line);

  // print on pm 2.5 readings on the second line
  sprintf(line, "PM 2.5: %d", pm25);
  lcd.setCursor(0,1);
  lcd.print(line);

  // send sensor readings via serial port on request
  if (Serial.available() > 0) {
    sprintf(serial, "%d %d %d\n", pm25, pm10, aqi);
    Serial.read();
    Serial.print(serial);
  }

  delay(1000);
}
