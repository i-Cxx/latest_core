cmake_minimum_required(VERSION 3.13)

# User Home (für VSCode Pico SDK Integration)
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# SDK Versionen (Pico SDK, Toolchain etc.)
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)

# VSCode Pico SDK Integration (optional)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Board (RP2040)
set(PICO_BOARD pico CACHE STRING "Board type")

# Pico SDK importieren
include(pico_sdk_import.cmake)

project(icxx_core_project C CXX ASM)

pico_sdk_init()

# FreeRTOS Port definieren (für RP2040 Cortex-M0+)
set(FREERTOS_PORT GCC_ARM_CM0 CACHE STRING "FreeRTOS port for RP2040 Cortex-M0+")

# freertos_config Interface-Library mit Pfad zu FreeRTOSConfig.h
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

# FreeRTOS Kernel als statische Library aus Quellen bauen
add_library(freertos_kernel
    external/FreeRTOS-Kernel/tasks.c
    external/FreeRTOS-Kernel/list.c
    external/FreeRTOS-Kernel/queue.c
    external/FreeRTOS-Kernel/timers.c
    external/FreeRTOS-Kernel/event_groups.c

    external/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    external/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c
    external/FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c
    external/FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

target_include_directories(freertos_kernel PUBLIC
    external/FreeRTOS-Kernel/include
    external/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${CMAKE_CURRENT_LIST_DIR}/src  # Für FreeRTOSConfig.h
)

target_link_libraries(freertos_kernel PUBLIC freertos_config)

# Andere Unterverzeichnisse (Libs, Src etc.)
add_subdirectory(lib)
add_subdirectory(src)
