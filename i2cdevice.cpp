#include "i2cdevice.h"

#include <cstddef>
#include <fcntl.h>
#include <filesystem>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <regex>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static __u8 brightness_to_magic(uint8_t brightness)
{
    if (brightness == 0)
        return 0;
    brightness = std::min(brightness, I2cDevice::max_brightness);
    return 0x20 - brightness;
}

I2cDevice::I2cDevice(const std::string &event_device)
{
    auto path = std::filesystem::canonical("/sys/class/input/" + event_device).string();
    std::regex i2c_re{"/(i2c-[0-9]+)/"};
    std::smatch match;
    if (not std::regex_search(path, match, i2c_re))
        throw std::runtime_error{"failed to get i2c device for event device " + event_device};
    std::string i2c_dev = "/dev/" + match[1].str();
    fd = open(i2c_dev.c_str(), O_RDWR);
    if (fd < 0)
        throw std::system_error{errno, std::generic_category(), "failed to open i2c device " + i2c_dev};
}
I2cDevice::~I2cDevice()
{
    close(fd);
}

void I2cDevice::set_brightness(uint8_t brightness)
{
    if (brightness == this->brightness)
        return;
    this->brightness = brightness;
    struct i2c_msg msg;
    msg.addr = 0x15;
    msg.flags = 0;
    __u8 data[] = {0x05, 0x00, 0x3d, 0x03, 0x06, 0x00, 0x07, 0x00, 0x0d, 0x14, 0x03, brightness_to_magic(brightness),
                   0xad};
    msg.buf = data;
    msg.len = sizeof(data);

    struct i2c_rdwr_ioctl_data rdwr
    {
        &msg, 1
    };
    int nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
    if (nmsgs_sent < 0)
        throw std::system_error{errno, std::generic_category(), "failed to set brightness"};
}
