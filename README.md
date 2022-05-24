# asus-numpad
Linux driver for Asus numpad

This projects lets you toggle between touchpad and numpad on Asus laptops.

## Setup
### Dependencies
* libevdev
* meson
* systemd (for automatic start)
* kernel module i2c_dev

### Build
```bash
meson setup build
meson compile -C build
meson install -C build
```

## Supported devices
* UX333FA

## Contributing
You are free to fork and propose pull requests.
Help is appreciated to port add support for other devices, improve documentation, package for distributions.

## Credits
Magic values, and general settings are from https://github.com/mohamed-badaoui/asus-touchpad-numpad-driver
