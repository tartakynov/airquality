#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

/*
US Standard
PM 2.5 (ug/m 3)                       AQI        Category
  0.0 -  12.0 (24hr)              0 -  50        Good
 12.1 -  35.4 (24hr)             51 - 100        Moderate
 35.5 -  55.4 (24hr)            101 - 150        Unhealthy for sensutive groups
 55.5 - 150.4 (24hr)            151 - 200        Unhealthy
150.5 - 250.4 (24hr)            201 - 300        Very Unhealth
250.5 - 350.4 (24hr)            301 - 400        Hazardous
350.5 - 500.4 (24hr)            401 - 500        Hazardous
*/
const float Cl_pm25[] = {0, 12.1, 35.5, 55.5, 150.5, 250.5, 350.5};
const float Ch_pm25[] = {12.0, 35.4, 55.4, 150.4, 250.4, 350.4, 500.4};
const float Il_pm25[] = {0, 51, 101, 151, 201, 301, 401};
const float Ih_pm25[] = {50, 100, 150, 200, 300, 400, 500};

/*
US Standard
PM 10 (ug/m 3)                        AQI        Category
  0 -  54 (24hr)                  0 -  50        Good
 55 - 154 (24hr)                 51 - 100        Moderate
155 - 254 (24hr)                101 - 150        Unhealthy for sensutive groups
255 - 354 (24hr)                151 - 200        Unhealthy
355 - 424 (24hr)                201 - 300        Very Unhealth
425 - 504 (24hr)                301 - 400        Hazardous
505 - 604 (24hr)                401 - 500        Hazardous
*/
const float Cl_pm10[] = {0, 55, 155, 255, 355, 425, 505};
const float Ch_pm10[] = {54, 154, 254, 354, 424, 504, 604};
const float Il_pm10[] = {0, 51, 101, 151, 201, 301, 401};
const float Ih_pm10[] = {50, 100, 150, 200, 300, 400, 500};

#define LIN_SIZE 16
#define SER_SIZE 64

char line[LIN_SIZE], serial[SER_SIZE];
int pm25 = 0, pm10 = 0, aqi = 0;

float getAQI(float c, float cl[], float ch[], float il[], float ih[], int n) {
  for (int i = 0; i < n; i++) {
    if (pm25 <= ch[i]) {
      return ((ih[i] - il[i]) / (ch[i] - cl[i])) * (c - cl[i]) + il[i];
    }
  }

  return -1;
}

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{
  pm25 = random(0, 100); // fake sensor input
  pm10 = 0; // fake sensor input
  aqi = max(
    getAQI(pm25, Cl_pm25, Ch_pm25, Il_pm25, Ih_pm25, 7),
    getAQI(pm10, Cl_pm10, Ch_pm10, Il_pm10, Ih_pm10, 7)
  );

  // print air quality index on the first line
  snprintf(line, LIN_SIZE, "AQI: %-8d", aqi);
  lcd.setCursor(3,0);
  lcd.print(line);

  // print on pm 2.5 readings on the second line
  snprintf(line, LIN_SIZE, "PM 2.5: %-8d", pm25);
  lcd.setCursor(0,1);
  lcd.print(line);

  // send sensor readings via serial port on request
  if (Serial.available() > 0) {
    snprintf(serial, SER_SIZE, "%d %d %d\n", pm25, pm10, aqi);
    Serial.read();
    Serial.print(serial);
  }

  delay(5000);
}
