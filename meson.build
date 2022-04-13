project('asus-numpad', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

evdev = dependency('libevdev')

conf_data = configuration_data()
conf_data.set_quoted('DATABASE_LOCATION', get_option('prefix') / get_option('datadir') / meson.project_name() / 'layouts.dat')
configure_file(output: 'config.h', configuration: conf_data)

configure_file(
  output: 'asus-numpad@.service',
  input: 'systemd/asus-numpad@.service.in',
  configuration: {'BIN': get_option('prefix') / get_option('bindir') / 'asus_numpad'},
  install: true,
  install_dir: 'lib/systemd/system'
)

install_data('layouts.dat')
install_data('udev/99-asus-numpad.rules',
  install_dir: 'lib/udev/rules.d')
install_data('systemd/asus-numpad.conf',
  install_dir: 'lib/modules-load.d')

exe = executable('asus_numpad',
  ['asus_numpad.cpp','layout.cpp', 'i2cdevice.cpp', 'virtualdevice.cpp'],
  dependencies : [evdev],
  install : true)

exe = executable('layout_test', ['layout_test.cpp','layout.cpp'],
  dependencies : [evdev])
