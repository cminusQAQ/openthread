#
#  Copyright (c) 2020, The OpenThread Authors.
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
    ${PROJECT_SOURCE_DIR}/src/posix/platform/include/openthread
    ${PROJECT_SOURCE_DIR}/src/posix/platform/include
    ${PROJECT_SOURCE_DIR}/src/posix/platform
)

target_compile_options(ot-ncp-client PRIVATE
    ${OT_CFLAGS}
)


target_link_libraries(ot-ncp-client
    pw_hdlc
    pw_stream
    util
    openthread-ncp-cli
    openthread-rpc-client
    openthread-platform
    pw_protobuf
    openthread-url
    pw_rpc.ncp_proto.nanopb
    pw_rpc.ncp_proto.nanopb_rpc
    ot-config
)

install(TARGETS ot-ncp-client DESTINATION bin)

