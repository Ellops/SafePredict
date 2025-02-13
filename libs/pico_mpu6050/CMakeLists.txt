pico_add_library(pico_mpu6050 NOFLAG)
target_sources(pico_mpu6050 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/mpu6050.c
        )
pico_mirrored_target_link_libraries(pico_mpu6050 INTERFACE
        pico_stdlib
        hardware_i2c
        )
target_include_directories(pico_mpu6050 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
