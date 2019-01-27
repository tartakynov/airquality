#ifndef SENSOR_H
#define SENSOR_H

#include "Stream.h"

class Sensor {
public:
  /**
   * PMS5003 data frame definition
   * http://www.aqmd.gov/docs/default-source/aq-spec/resources-page/plantower-pms5003-manual_v2-3.pdf
   */
  struct DATA {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
  };

  Sensor(Stream&);

  /**
   * Reads all the available frames until the stream is dry.
   * Returns true if at least one frame is read successfully.
   * DATA will contain the latest successfully read data frame.
   */
  bool read(DATA&);

private:
  Stream* stream;
  uint8_t buffer[30];
  uint16_t buffer_u16[15];
};

#endif
