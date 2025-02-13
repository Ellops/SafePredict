pico_add_library(pico_buzzer NOFLAG)
target_sources(pico_buzzer INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/buzzer.c
        )
pico_mirrored_target_link_libraries(pico_buzzer INTERFACE
        pico_stdlib
        hardware_clocks
        hardware_pwm
        )
target_include_directories(pico_buzzer INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
