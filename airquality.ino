#include <LiquidCrystal_I2C.h>

#define LIN_SIZE 16
#define SER_SIZE 64
#define LEVELS 7

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

/*
US Standard
PM 2.5 (ug/m 3)                       AQI        Category
---------------------------------------------------------
  0.0 -  12.0 (24hr)              0 -  50        Good
 12.1 -  35.4 (24hr)             51 - 100        Moderate
 35.5 -  55.4 (24hr)            101 - 150        Unhealthy for sensutive groups
 55.5 - 150.4 (24hr)            151 - 200        Unhealthy
150.5 - 250.4 (24hr)            201 - 300        Very Unhealth
250.5 - 350.4 (24hr)            301 - 400        Hazardous
350.5 - 500.4 (24hr)            401 - 500        Hazardous
*/
const int Cl_pm25[] = {0, 12, 35, 55, 150, 250, 350};
const int Ch_pm25[] = {12, 35, 55, 150, 250, 350, 500};
const int Il_pm25[] = {0, 51, 101, 151, 201, 301, 401};
const int Ih_pm25[] = {50, 100, 150, 200, 300, 400, 500};

/*
US Standard
PM 10 (ug/m 3)                        AQI        Category
---------------------------------------------------------
  0 -  54 (24hr)                  0 -  50        Good
 55 - 154 (24hr)                 51 - 100        Moderate
155 - 254 (24hr)                101 - 150        Unhealthy for sensutive groups
255 - 354 (24hr)                151 - 200        Unhealthy
355 - 424 (24hr)                201 - 300        Very Unhealth
425 - 504 (24hr)                301 - 400        Hazardous
505 - 604 (24hr)                401 - 500        Hazardous
*/
const int Cl_pm100[] = {0, 55, 155, 255, 355, 425, 505};
const int Ch_pm100[] = {54, 154, 254, 354, 424, 504, 604};
const int Il_pm100[] = {0, 51, 101, 151, 201, 301, 401};
const int Ih_pm100[] = {50, 100, 150, 200, 300, 400, 500};

const LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

const char line[LIN_SIZE], serial[SER_SIZE];

const char* mood[] = {
  "GOOD",
  "MODERATE",
  "SENSITIVE",
  "UNHEALTHY",
  "VERY UNHEALTHY",
  "HAZARDOUS",
  "HAZARDOUS"
};

struct pms5003data data;

boolean readPMSdata(Stream *s) {
  static uint8_t buffer[30];
  static uint16_t buffer_u16[15];
  static uint16_t sum;

  while (s->available() >= 32) {
    if (s->read() == 0x42) {
      if (s->peek() == 0x4D) {
        s->read();
      } else {
        continue;
      }

      s->readBytes(buffer, 30);

      // get checksum ready
      sum = 0x8F;
      for (uint8_t i=0; i < 28; i++) {
        sum += buffer[i];
      }

      // The data comes in endian'd, this solves it
      for (uint8_t i=0; i < 15; i++) {
        buffer_u16[i] = buffer[i*2 + 1];
        buffer_u16[i] += (buffer[i*2] << 8);
      }

      memcpy((void *)&data, (void *)buffer_u16, 30);
      // Serial.print(data.checksum);
      // Serial.print(" ");
      // Serial.println(sum);
    }
  }

  return sum == data.checksum;
}

int computeAQI(int c, int cl[], int ch[], int il[], int ih[], int &level) {
  for (level = 0; level < LEVELS; level++) {
    if (c <= ch[level]) {
      return (float(ih[level] - il[level]) / (ch[level] - cl[level])) * (c - cl[level]) + il[level];
    }
  }

  return -1;
}

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

void loop()
{
  int l1, l2;
  if (readPMSdata(&Serial)) {
    int aqi = max(
      computeAQI(data.pm25_env, Cl_pm25, Ch_pm25, Il_pm25, Ih_pm25, l1),
      computeAQI(data.pm100_env, Cl_pm100, Ch_pm100, Il_pm100, Ih_pm100, l2)
    );
    int level = max(l1, l2);

    // print air quality index on the first line
    snprintf(line, LIN_SIZE + 1, "%d %-16s", aqi, mood[level]);
    lcd.setCursor(0, 0);
    lcd.print(line);

    // print pm 2.5 readings on the second line
    snprintf(line, LIN_SIZE + 1, "pm2.5 %-8d", data.pm25_env);
    lcd.setCursor(0, 1);
    lcd.print(line);
  }

  delay(2000);
}
