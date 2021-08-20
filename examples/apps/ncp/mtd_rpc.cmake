add_executable(ot-ncp-mtd-rpc
    main.c
    ncp.c
)

target_include_directories(ot-ncp-mtd-rpc PRIVATE ${COMMON_INCLUDES})

target_link_libraries(ot-ncp-mtd-rpc PRIVATE
    openthread-rpc-server
    openthread-ncp-mtd
    ${OT_PLATFORM_LIB}
    openthread-mtd
    ${OT_PLATFORM_LIB}
    openthread-ncp-mtd
    ${OT_MBEDTLS}
    ot-config
)

install(TARGETS ot-ncp-mtd-rpc DESTINATION bin)
