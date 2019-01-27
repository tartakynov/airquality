#include "sensor.h"

Sensor::Sensor(Stream& stream)
{
  this->stream = &stream;
}

bool Sensor::read(DATA& data) {
  bool success = false;

  // read everything from the stream until it's dry
  while (stream->available() >= 32) {
    // read the stream one by one until the start characters (0x42 0x4D)
    if (stream->read() == 0x42 && stream->peek() == 0x4D) {
      stream->read();
      stream->readBytes(buffer, 30);

      // compute the checksum
      uint16_t sum = 0x8F;
      for (uint8_t i = 0; i < 28; i++) {
        sum += buffer[i];
      }

      // convert the endian
      for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[i*2 + 1];
        buffer_u16[i] += (buffer[i*2] << 8);
      }

      if (sum == buffer_u16[14]) {
        memcpy((void *)&data, (void *)buffer_u16, 30);
        success = true;
      }
    }
  }

  return success;
}
