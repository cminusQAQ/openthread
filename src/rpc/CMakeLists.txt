#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

include($ENV{OT_PIGWEED_DIR}/pw_build/pigweed.cmake)
include($ENV{OT_PIGWEED_DIR}/pw_protobuf_compiler/proto.cmake)

set(dir_pw_third_party_nanopb $ENV{OT_NANOPB_DIR} CACHE PATH "path to nanopb library" FORCE)

pw_set_backend(pw_assert pw_assert_log)
pw_set_backend(pw_log pw_log_basic)
pw_set_backend(pw_sys_io pw_sys_io_stdio)

add_subdirectory($ENV{OT_PIGWEED_DIR} pigweed.out)
add_subdirectory($ENV{OT_NANOPB_DIR} nanopb.out)

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER g++)

set(COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/posix
    ${PROJECT_SOURCE_DIR}/src/posix/platform
    ${PROJECT_SOURCE_DIR}/src/posix/platform/include
)

pw_proto_library(pw_rpc.ncp_proto
    SOURCES
        proto/ncp.proto
    INPUTS
        proto/ncp.options
)

set(PIGWEED_LIBS
    mbedtls
    pw_hdlc
    pw_rpc.ncp_proto.nanopb_rpc
    pw_string
)

add_library(openthread-rpc-client
    common.hpp
    rpc_channel_output.hpp
    rpc_client.cpp
)

target_include_directories(openthread-rpc-client PUBLIC
    ${COMMON_INCLUDES}
    ${OT_PUBLIC_INCLUDES}
)


target_link_libraries(openthread-rpc-client PRIVATE 
    ${PIGWEED_LIBS}
    ot-config
)

target_compile_options(openthread-rpc-client PRIVATE
    ${OT_CFLAGS}
)

add_library(openthread-rpc-server
    common.hpp
    ncp_service.cpp
    rpc_channel_output.hpp
    rpc_server.cpp
)

target_include_directories(openthread-rpc-server PUBLIC
    ${COMMON_INCLUDES}
    ${OT_PUBLIC_INCLUDES}
)

target_link_libraries(openthread-rpc-server PRIVATE 
    ${PIGWEED_LIBS}
    ot-config
)

target_compile_options(openthread-rpc-server PRIVATE
    ${OT_CFLAGS}
)

add_library(openthread-ncp-cli
    ../core/api/error_api.cpp
    ../core/common/error.cpp
    ../core/common/string.cpp
    ../core/utils/lookup_table.cpp
    ../core/utils/parse_cmdline.cpp
    ncp_cli.cpp
)

target_include_directories(openthread-ncp-cli PUBLIC
    ${COMMON_INCLUDES}
    ${OT_PUBLIC_INCLUDES}
)

target_link_libraries(openthread-ncp-cli PRIVATE 
    ${PIGWEED_LIBS}
    ot-config
)

target_compile_options(openthread-ncp-cli PRIVATE
    ${OT_CFLAGS}
)
