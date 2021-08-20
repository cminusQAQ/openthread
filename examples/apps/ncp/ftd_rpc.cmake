add_executable(ot-ncp-ftd-rpc
    main.c
    ncp.c
)

target_compile_definitions(ot-ncp-ftd-rpc
    PRIVATE
        OPENTHREAD_RPC_FTD=1
)

target_include_directories(ot-ncp-ftd-rpc PRIVATE ${COMMON_INCLUDES})

target_link_libraries(ot-ncp-ftd-rpc PRIVATE
    openthread-rpc-server
    openthread-ncp-ftd
    ${OT_PLATFORM_LIB}
    openthread-ftd
    ${OT_PLATFORM_LIB}
    openthread-ncp-ftd
    ${OT_MBEDTLS}
    ot-config
)

install(TARGETS ot-ncp-ftd-rpc DESTINATION bin)
