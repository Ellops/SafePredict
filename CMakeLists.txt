# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

set(PICO_DISABLE_LWIP_TLS ON)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(SafePredict C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(SafePredict SafePredict.c )

set(ENV_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config/.env")
if(EXISTS ${ENV_FILE})
    file(READ ${ENV_FILE} ENV_CONTENTS)

    # Extract values from .env (Regex-based)
    string(REGEX MATCH "WIFI_SSID=\"([^\"]+)\"" _ ${ENV_CONTENTS})
    set(WIFI_SSID ${CMAKE_MATCH_1})

    string(REGEX MATCH "WIFI_PASSWORD=\"([^\"]+)\"" _ ${ENV_CONTENTS})
    set(WIFI_PASSWORD ${CMAKE_MATCH_1})
endif()

target_compile_definitions(SafePredict PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

pico_set_program_name(SafePredict "SafePredict")
pico_set_program_version(SafePredict "0.1")

target_include_directories(SafePredict PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/config
        )

pico_enable_stdio_uart(SafePredict 0)
pico_enable_stdio_usb(SafePredict 1)

add_subdirectory(libs/pico_http_request)

target_link_libraries(SafePredict
            pico_stdlib
            pico_lwip_http
            pico_lwip_mbedtls
            pico_mbedtls
            pico_cyw43_arch_lwip_threadsafe_background
            pico_http_request
            )

pico_add_extra_outputs(SafePredict)