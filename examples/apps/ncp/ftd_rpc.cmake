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
    pw_stream
    pw_hdlc
    pw_sys_io
    pw_sys_io_stdio
    pw_rpc.common
    util
    pw_rpc.client
    pw_rpc.server
    openthread-ncp-cli
    openthread-rpc-client
    openthread-platform
    pw_protobuf
    openthread-url
    pw_rpc.nanopb.method
    pw_rpc.ncp_proto.nanopb
    pw_rpc.ncp_proto.nanopb_rpc
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