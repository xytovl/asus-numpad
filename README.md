# asus-numpad
Linux driver for Asus numpad

This projects lets you toggle between touchpad and numpad on Asus laptops.

## Setup
### Dependencies
* libevdev
* meson
* pkg-config
* systemd (for automatic start)
* kernel module i2c_dev

### Build
```bash
meson setup build
meson compile -C build
meson install -C build
```

## Supported numpads
Like **UX333FA**:

|   |   |   |   | ⌨ |
|---|---|---|---|---|
| 7 | 8 | 9 | / | ⌫ |
| 4 | 5 | 6 | * | ⌫ |
| 1 | 2 | 3 | - | ↵ |
| 0 | 0 | . | + | ↵ |

Like **UX434IQ**:

| ⌘ |   |   |   | ⌨ |
|---|---|---|---|---|
| 7 | 8 | 9 | / | ⌫ |
| 4 | 5 | 6 | * | ⌫ |
| 1 | 2 | 3 | - | % |
| 0 | . | ↵ | + | = |

## Add layout
Layout config stores in `layouts.dat`. Edit it to add a layout.<br>
There are next fields:

* **match**<br>
Match layout of numpad<br>
Value from `/sys/class/dmi/id/board_name`.<br>
Example: `match = UX434IQ M433IA`

* **row**<br>
Row of keys for libevdev.<br>
Taken from `linux/input-event-codes.h`<br>
Example: `row = KEY_KP7 KEY_KP8   KEY_KP9   KEY_KPSLASH    KEY_BACKSPACE`

* **pad_padding**<br>
Numpad area with keys.<br>
Example: `pad_padding = 0.152 0.069 0.06 0.062`


For UX434IQ there is next calcalation of **pad_padding**<br>
```
total_height   = 66mm  (100%  = 1)
total_width    = 130mm (100%  = 1)
top_padding    = 10mm  (10/66 = 15.2% = 0.152)
right_padding  = 9mm   (9/130 = 6.9%  = 0.069)
bottom_padding = 4mm   (4/66  = 6%    = 0.06)
left_padding   = 8mm   (8/130 = 6.2%  = 0.062)
```

## Contributing
You are free to fork and propose pull requests.
Help is appreciated to port add support for other devices, improve documentation, package for distributions.

## Credits
Magic values, and general settings are from https://github.com/mohamed-badaoui/asus-touchpad-numpad-driver
