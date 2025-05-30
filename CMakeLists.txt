# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(autocar C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(autocar
    autocar.cpp
    Driver.cpp
    Motor.cpp
    I2CMultiplexer.cpp
    DistanceSensor.cpp
    SpeedController.cpp
    vl6180x/platform/cci-i2c/vl6180x_i2c.c
    vl6180x/src/vl6180x_api.c
    )

# pull in common dependencies
target_link_libraries(autocar pico_stdlib hardware_pwm pico_stdio_usb hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(autocar)

# usb
pico_enable_stdio_usb(autocar 1)
pico_enable_stdio_uart(autocar 0)

# add url via pico_set_program_url
