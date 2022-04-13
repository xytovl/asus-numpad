#pragma once

class Layout;
struct libevdev;
struct libevdev_uinput;

class VirtualDevice
{
  public:
    VirtualDevice(const Layout &layout);
    ~VirtualDevice();

    void send(unsigned int type, unsigned int code, int value);

  private:
    libevdev *handle;
    libevdev_uinput *uidev;
};
