#include "config.h"
#include "i2cdevice.h"
#include "layout.h"
#include "virtualdevice.h"

#include <fcntl.h>
#include <iostream>
#include <libevdev/libevdev.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class Numpad
{
  public:
    Numpad(const std::filesystem::path &filename, const Layout &l) : i2c{filename.filename()}, layout{l}, numpad{l}
    {
        fd = open(filename.c_str(), O_RDONLY);
        int rc = libevdev_new_from_fd(fd, &handle);
        if (rc < 0)
        {
            throw std::system_error{-rc, std::generic_category(), "failed to init libevdev"};
        }
        info_x = libevdev_get_abs_info(handle, ABS_X);
        if (info_x == nullptr)
        {
            throw std::runtime_error{"failed to get X axis info for " + filename.string()};
        }
        info_y = libevdev_get_abs_info(handle, ABS_Y);
        if (info_y == nullptr)
        {
            throw std::runtime_error{"failed to get Y axis info for " + filename.string()};
        }
        btn_x = info_x->maximum * 0.95;
        btn_y = info_y->maximum * 0.07;
    }
    ~Numpad()
    {
        libevdev_free(handle);
        close(fd);
    }

    void run()
    {
        int32_t x, y;
        enum class Mode
        {
            Touchpad,
            Transition,
            VirtualDevice,
        };
        Mode mode = Mode::Touchpad;
        uint8_t brightness = I2cDevice::max_brightness;
        int pressed = 0;
        while (true)
        {
            input_event ev;
            int rc = libevdev_next_event(handle, LIBEVDEV_READ_FLAG_NORMAL, &ev);
            if (rc < 0)
            {
                throw std::system_error{-rc, std::generic_category(), "error in libevdev_next_event"};
            }
            switch (ev.type)
            {
            case EV_ABS:
                switch (ev.code)
                {
                case ABS_MT_POSITION_X:
                    x = ev.value;
                    if (mode == Mode::Transition)
                    {
                        if (x < btn_x)
                        {
                            brightness = 1 + (I2cDevice::max_brightness * x) / btn_x;
                            i2c.set_brightness(brightness);
                        }
                    }
                    break;
                case ABS_MT_POSITION_Y:
                    y = ev.value;
                    break;
                }
                break;
            case EV_KEY:
                switch (ev.code)
                {
                case BTN_TOOL_FINGER:
                    if (ev.value == 0 and pressed != 0)
                    {
                        // Release event
                        numpad.send(EV_KEY, pressed, 0);
                        pressed = 0;
                    }
                    if (ev.value == 1 and x > btn_x and y < btn_y)
                    {
                        // Top-right corner: mode change
                        switch (mode)
                        {
                        case Mode::Touchpad:
                            i2c.set_brightness(brightness);
                            mode = Mode::Transition;
                            numpad.send(EV_KEY, KEY_NUMLOCK, 1);
                            libevdev_grab(handle, LIBEVDEV_GRAB);
                            break;
                        case Mode::Transition:
                            break;
                        case Mode::VirtualDevice:
                            i2c.set_brightness(0);
                            mode = Mode::Touchpad;
                            numpad.send(EV_KEY, KEY_NUMLOCK, 0);
                            libevdev_grab(handle, LIBEVDEV_UNGRAB);
                        }
                    }
                    else if (ev.value == 0 and mode == Mode::Transition)
                    {
                        // Button release after mode-change initiated
                        mode = Mode::VirtualDevice;
                    }
                    else if (ev.value == 1 and mode == Mode::VirtualDevice)
                    {
                        // Numpad mode, button press
                        pressed = layout(double(x) / info_x->maximum, double(y) / info_y->maximum);
                        if (pressed != 0)
                        {
                            numpad.send(EV_KEY, pressed, 1);
                        }
                    }
                    break;
                }
            }
        }
    }

  private:
    int fd;
    libevdev *handle;
    int32_t btn_x;
    int32_t btn_y;
    const input_absinfo *info_x;
    const input_absinfo *info_y;
    I2cDevice i2c;
    const Layout &layout;
    VirtualDevice numpad;
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " FILE [model]" << std::endl
                  << "where FILE is the event device such as /dev/input/event0" << std::endl
                  << "and model is the model name, if omitted it is read from /sys/class/dmi/id/board_name"
                  << std::endl;
        return 1;
    }
    try
    {
        Layout l = parse_layout(DATABASE_LOCATION);
        Numpad numpad(argv[1], l);
        numpad.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
