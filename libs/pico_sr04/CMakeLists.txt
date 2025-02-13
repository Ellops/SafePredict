pico_add_library(pico_sr04 NOFLAG)
target_sources(pico_sr04 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/distance_sr04.c
        )
pico_mirrored_target_link_libraries(pico_sr04 INTERFACE
        pico_stdlib
        hardware_clocks
        hardware_pwm
        )
target_include_directories(pico_sr04 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
