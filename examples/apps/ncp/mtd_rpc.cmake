add_executable(ot-ncp-mtd-rpc
    main.c
    ncp.c
)
target_include_directories(ot-ncp-mtd-rpc PRIVATE ${COMMON_INCLUDES})
target_link_libraries(ot-ncp-mtd-rpc PRIVATE
    pw_stream
    pw_hdlc
    pw_sys_io
    pw_sys_io_stdio
    pw_rpc.common
    util
    pw_rpc.client
    pw_rpc.server
    openthread-ncp-cli
    pw_rpc.nanopb.method
    openthread-rpc-client
    openthread-platform
    pw_protobuf
    openthread-url
    pw_rpc.ncp_proto.nanopb
    pw_rpc.ncp_proto.nanopb_rpc
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