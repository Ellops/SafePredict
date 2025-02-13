pico_add_library(pico_wifi_manager NOFLAG)
target_sources(pico_wifi_manager INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/wifi_manager.c
        )
pico_mirrored_target_link_libraries(pico_wifi_manager INTERFACE
        pico_cyw43_arch_lwip_threadsafe_background
        )
target_include_directories(pico_wifi_manager INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
