# Chatgpt - Verlauf

Sehr gute Idee! Eine saubere Projektstruktur und eine klare Checkliste sorgen für langfristige Wartbarkeit und Übersichtlichkeit – besonders bei eingebetteten Projekten wie mit dem RP2040, FreeRTOS und CMake.

---

## ✅ **1. Ziel:**

Ein **Raspberry Pi Pico C/C++ Projekt mit FreeRTOS**, Treiber in `lib/`, Code in `src/`, externe Abhängigkeiten in `external/`. Die Projektstruktur soll sauber, modular und CMake-basiert sein.

---

## 📁 **2. Zielstruktur deines Projekts**

```
icxx_core/
├── CMakeLists.txt                  # Top-Level CMake
├── pico_sdk_import.cmake          # Offiziell von pico-sdk
├── README.md                      # Projektbeschreibung
│
├── build/                         # Buildverzeichnis (per .gitignore ignorieren)
│   └── compile_commands.json      # Optional: Für Language Server
│
├── external/                      # Externe Repositories als Submodules
│   └── FreeRTOS-Kernel/           # FreeRTOS Submodule (nur Kernel)
│       └── ...                    # FreeRTOS Quellen, "portable/", "include/", etc.
│
├── lib/                           # Eigene Treiberbibliotheken
│   ├── CMakeLists.txt             # Fügt alle Libs ein
│   ├── lcd_1602_i2c/
│   │   ├── lcd_1602_i2c.h
│   │   ├── lcd_1602_i2c.c
│   │   └── CMakeLists.txt
│   └── ssd1306_i2c/
│       ├── ssd1306_i2c.h
│       ├── ssd1306_i2c.c
│       └── CMakeLists.txt
│
├── src/                           # Hauptquellen
│   ├── FreeRTOSConfig.h           # Board-spezifische FreeRTOS-Konfiguration
│   ├── icxx_core.c                # Main
│   └── CMakeLists.txt             # Erzeugt Executable
```

---

## ⚙️ **3. Notwendige Schritte (einmalig)**

### 🔧 Initial Setup:

1. `git init`
2. `git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel external/FreeRTOS-Kernel`
3. `git submodule update --init --recursive`

---

### 📄 `CMakeLists.txt` (Root)

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(icxx_core C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# SDK vorbereiten
pico_sdk_init()

# Unterverzeichnisse
add_subdirectory(lib)
add_subdirectory(src)

# FreeRTOS Kernel einbinden
add_subdirectory(external/FreeRTOS-Kernel)
```

---

### 📄 `lib/CMakeLists.txt`

```cmake
add_subdirectory(lcd_1602_i2c)
add_subdirectory(ssd1306_i2c)
```

---

### 📄 `src/CMakeLists.txt`

```cmake
add_executable(icxx_core
    icxx_core.c
)

pico_set_program_name(icxx_core "icxx_core")
pico_set_program_version(icxx_core "0.1")

# IO deaktivieren falls nötig
pico_enable_stdio_uart(icxx_core 0)
pico_enable_stdio_usb(icxx_core 0)

target_link_libraries(icxx_core
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_timer

    lcd_1602_i2c_lib
    ssd1306_i2c_lib

    FreeRTOS-Kernel
)

target_include_directories(icxx_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
pico_add_extra_outputs(icxx_core)
```

---

## 📋 **4. Checkliste (Kontrolle vor Build)**

| ✅  | Punkt                                                            |
| -- | ---------------------------------------------------------------- |
| 🔲 | Projektverzeichnis `icxx_core/` erstellt                         |
| 🔲 | `pico_sdk_import.cmake` vorhanden                                |
| 🔲 | `external/FreeRTOS-Kernel` mit `git submodule` eingebunden       |
| 🔲 | `src/FreeRTOSConfig.h` vorhanden und an Board angepasst          |
| 🔲 | `src/icxx_core.c` mit `main()` vorhanden                         |
| 🔲 | Treiber in `lib/` jeweils mit eigenem `CMakeLists.txt`           |
| 🔲 | Root `CMakeLists.txt` enthält alle `add_subdirectory()` Einträge |
| 🔲 | FreeRTOS wird via `add_subdirectory()` eingebunden               |
| 🔲 | CMake wurde erfolgreich konfiguriert (`cmake ..`)                |
| 🔲 | Build erfolgreich (`cmake --build .`)                            |
| 🔲 | `.uf2` Datei erzeugt in `build/` (z. B. `icxx_core.uf2`)         |

---

Wenn du möchtest, kann ich dir auch eine kleine Vorlage für `FreeRTOSConfig.h` für den RP2040 schreiben.

Möchtest du das?
