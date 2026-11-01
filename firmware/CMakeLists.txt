cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ble_hardware_template)
target_sources(app PRIVATE
  src/main.c src/ble/advertising.c src/ble/service.c
  src/protocol/codec.c src/application/device.c
  src/acquisition/state.c src/acquisition/service.c
  src/hardware/simulated.c src/hardware/stub.c)
