# Declare the library
add_library(pico_ssd1306 STATIC
    ssd1306.c
    ssd1306.h
)

# Add the include directories
target_include_directories(pico_ssd1306 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link required libraries
target_link_libraries(pico_ssd1306
    pico_stdlib            # For standard Pico functions
    hardware_i2c           # For I2C hardware functionality
    pico_binary_info       # For binary info support
)
