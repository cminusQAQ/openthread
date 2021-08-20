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

add_executable(ot-ncp-client
    NcpClient.cpp
    platform/uart.cpp
)

target_include_directories(ot-ncp-client
    PRIVATE 
        ${COMMON_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/posix/platform
        ${PROJECT_SOURCE_DIR}/src/posix/platform/include
        ${PROJECT_SOURCE_DIR}/src/posix/platform/include/openthread
)

target_compile_options(ot-ncp-client PRIVATE
    ${OT_CFLAGS}
)

target_link_libraries(ot-ncp-client
    openthread-ncp-cli
    openthread-platform
    openthread-rpc-client
    openthread-url
    ot-config
    pw_hdlc
    pw_protobuf
    pw_rpc.ncp_proto.nanopb
    pw_rpc.ncp_proto.nanopb_rpc
    pw_stream
    util
)

install(TARGETS ot-ncp-client DESTINATION bin)
