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

#include "rpc_config.h"
#include "pw_rpc/channel.h"
#include "pw_status/status.h"
#include "pw_stream/sys_io_stream.h"

namespace ot {
namespace Rpc {
class RpcChannelOutput : public pw::rpc::ChannelOutput
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in] writer       A pigweed stream writer.
     * @param[in] address      The RPC address.
     * @param[in] ChannelName  The channel name.
     *
     */
    RpcChannelOutput(pw::stream::Writer &writer, uint8_t address, const char *aChannelName)
        : pw::rpc::ChannelOutput(aChannelName)
        , mWriter(writer)
        , mAddress(address)
    {
    }

    /**
     * This method get the buffer stored in mBuffer.
     *
     * @returns The buffer stored in mBuffer.
     *
     */
    std::span<std::byte> AcquireBuffer() override { return mBuffer; }

    /**
     * Sends the contents of a buffer previously obtained from AcquireBuffer().
     *
     * @param[in] buffer A byte std::span. This may be called with an empty span, in which
     *                   case the buffer should be released without sending any data.
     *
     * @returns Returns OK if the operation succeeded, or an implementation-defined Status
     *          value if there was an error. The implementation must NOT return
     *          FAILED_PRECONDITION or INTERNAL, which are reserved by pw_rpc.
     *
     */
    pw::Status SendAndReleaseBuffer(std::span<const std::byte> buffer) override
    {
        PW_DASSERT(buffer.data() == mBuffer.data());
        return buffer.empty() ? pw::OkStatus() : pw::hdlc::WriteUIFrame(mAddress, buffer, mWriter);
    }

private:
    enum
    {
        kMaxBufferSize = OPENTHREAD_CONFIG_RPC_MAX_BUFFER_SIZE,
    };
    pw::stream::Writer &                  mWriter;
    std::array<std::byte, kMaxBufferSize> mBuffer;
    const uint8_t                         mAddress;
};
} // namespace Rpc
} // namespace ot
