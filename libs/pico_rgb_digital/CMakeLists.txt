pico_add_library(pico_rgb_digital NOFLAG)
target_sources(pico_rgb_digital INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/rgb_digital.c
        )
pico_mirrored_target_link_libraries(pico_rgb_digital INTERFACE
        pico_stdlib
        )
target_include_directories(pico_rgb_digital INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
