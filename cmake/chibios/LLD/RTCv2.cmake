target_sources(qmk PUBLIC ${CHIBIOS}/os/hal/ports/${MCU_PORT_NAME}/LLD/RTCv2/hal_rtc_lld.c)
target_include_directories(qmk PUBLIC ${CHIBIOS}/os/hal/ports/${MCU_PORT_NAME}/LLD/RTCv2)