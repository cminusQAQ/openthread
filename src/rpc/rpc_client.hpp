/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/client.h"

#include "rpc_channel_output.hpp"
#include "rpc_config.h"
#include "pw_rpc/channel.h"
#include "pw_status/status.h"
#include "pw_stream/sys_io_stream.h"

namespace ot {
namespace Rpc {

class RpcClient
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance  The OpenThread instance structure.
     * @param[in]  aWriter    A pigweed stream writer structure.
     *
     */
    RpcClient(pw::stream::Writer *aWriter);

    /**
     * This method will be called to process an RPC packet when the transport layer receives one.
     *
     * @param[in] aBuffer       A pointer to string.
     * @param[in] aLength       The length of the string.
     *
     */
    void ProcessPackets(const uint8_t *aBuffer, uint16_t aLength);

    /**
     * This method will provide the channel stored in mChannel.
     *
     * @returns  The channel stored in mChannel.
     *
     */
    pw::rpc::Channel &GetChannel(void);

private:
    enum
    {
        kMaxBufferSize = OPENTHREAD_CONFIG_RPC_MAX_BUFFER_SIZE,
    };
    RpcChannelOutput                      mChannelOutput;
    pw::rpc::Channel                      mChannels[1];
    pw::rpc::Client                       mClient;
    pw::hdlc::Decoder                     mDecoder;
    std::array<std::byte, kMaxBufferSize> mDecodeBuffer;
};

} // namespace Rpc
} // namespace ot
