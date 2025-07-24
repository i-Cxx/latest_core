# Top-Level CMakeLists.txt für Raspberry Pi Pico + Treiber + optional FreeRTOS

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setze Board (z. B. pico, pico_w, etc.)
set(PICO_BOARD pico CACHE STRING "Board type")

# == VSCode-Erweiterung: Integration (nicht ändern) ==
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
# =====================================================

# Lade Pico SDK (lokal via Submodul)
include(pico_sdk_import.cmake)

project(icxx_core_project C CXX ASM)

# Initialisiere SDK
pico_sdk_init()

# Optional: FreeRTOS hinzufügen
# add_subdirectory(external/FreeRTOS-Kernel)
add_subdirectory(external/FreeRTOS-Kernel)

# Bibliotheken (Treiber)
add_subdirectory(lib)

# Hauptquelle
add_subdirectory(src)
