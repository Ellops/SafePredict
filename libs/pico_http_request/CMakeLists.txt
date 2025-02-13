pico_add_library(pico_http_request NOFLAG)
target_sources(pico_http_request INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/http_request.c
        )
pico_mirrored_target_link_libraries(pico_http_request INTERFACE
        pico_lwip_http
        pico_lwip_mbedtls
        pico_mbedtls
        )
target_include_directories(pico_http_request INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
