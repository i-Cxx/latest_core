# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Dies ist die CMakeLists.txt fÃ¼r den Ã¼bergeordneten 'librarys'-Ordner.
# Sie bindet alle einzelnen Bibliotheken in diesem Verzeichnis ein.

# Binde die LCD-Bibliothek ein
add_subdirectory(lcd_1602_i2c)

# Binde die SSD1306-Bibliothek ein
add_subdirectory(ssd1306_i2c)

# Binde Blink Biliothek ein
#add_subdirectory(blink)

