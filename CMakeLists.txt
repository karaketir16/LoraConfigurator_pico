cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_subdirectory(LoRa_E32_Series_Library_Pico_SDK)
# rest of your project


if (TARGET tinyusb_device)
    add_executable(hello_usb
            hello_usb.c
            main.cpp
            )

    # pull in common dependencies
    target_link_libraries(hello_usb 
    pico_stdlib
    pico_multicore
    )
    

    # enable usb output, disable uart output
    pico_enable_stdio_usb(hello_usb 1)
    pico_enable_stdio_uart(hello_usb 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(hello_usb)

    # add url via pico_set_program_url
    # example_auto_set_url(hello_usb)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()


target_link_libraries(hello_usb lora_e32)