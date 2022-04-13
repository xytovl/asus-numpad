#include "virtualdevice.h"

#include "layout.h"

#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

VirtualDevice::VirtualDevice(const Layout &layout)
{
    handle = libevdev_new();
    libevdev_set_name(handle, "Virtual numpad");
    libevdev_enable_event_type(handle, EV_KEY);
    libevdev_enable_event_code(handle, EV_KEY, KEY_NUMLOCK, nullptr);
    for (auto key : layout)
        libevdev_enable_event_code(handle, EV_KEY, key, nullptr);
    int err = libevdev_uinput_create_from_device(handle, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0)
    {
        throw std::runtime_error{"libevdev_uinput_create_from_device failed"};
    }
}
VirtualDevice::~VirtualDevice()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(handle);
}

void VirtualDevice::send(unsigned int type, unsigned int code, int value)
{
    int err = libevdev_uinput_write_event(uidev, type, code, value);
    if (err != 0)
        throw std::system_error{-err, std::generic_category(), "libevdev_uinput_write_event failed"};
    err = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    if (err != 0)
        throw std::system_error{-err, std::generic_category(), "libevdev_uinput_write_event failed"};
}
