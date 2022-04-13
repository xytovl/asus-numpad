#pragma once

#include <cstdint>
#include <string>

class I2cDevice
{
  public:
    inline static const uint8_t max_brightness = 16;

    I2cDevice(const std::string &event_device);
    ~I2cDevice();

    void set_brightness(uint8_t brightness);

  private:
    int fd;
    uint8_t brightness = 0;
};

