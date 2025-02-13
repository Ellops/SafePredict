# Declare the library
add_library(pico_http_request STATIC
    http_request.c
    http_request.h
)

# Add the include directories
target_include_directories(pico_http_request PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/../../config)

# Link required libraries
target_link_libraries(pico_http_request
    pico_lwip_http
    pico_lwip_mbedtls
    pico_mbedtls
)
