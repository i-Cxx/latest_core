add_executable(icxx_core
    icxx_core.c
)

pico_set_program_name(icxx_core "icxx_core")
pico_set_program_version(icxx_core "0.1")

pico_enable_stdio_uart(icxx_core 0)
pico_enable_stdio_usb(icxx_core 0)

target_link_libraries(icxx_core
    pico_stdio
    pico_stdlib
    hardware_gpio
    hardware_spi
    hardware_i2c
    hardware_timer

    lcd_1602_i2c_lib
    ssd1306_i2c_lib

    freertos_kernel
    freertos_config
)

target_include_directories(icxx_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

pico_add_extra_outputs(icxx_core)
