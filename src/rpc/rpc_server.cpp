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

#include "rpc_server.hpp"
#include "common.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"

namespace ot {
namespace Rpc {

pw::Status UartWriter::DoWrite(std::span<const std::byte> data)
{
    uint8_t  buf[kMaxBufferSize] = {0};
    uint16_t size                = data.size();

    for (uint16_t i = 0; i < size; i++)
    {
        uint16_t pos = i % kMaxBufferSize;
        buf[pos]     = (uint8_t)data[i];

        if (pos == kMaxBufferSize - 1 || pos == size - 1)
        {
            mCallback(buf, pos + 1);
        }
    }
    return pw::OkStatus();
}

RpcServer::RpcServer(otInstance *aInstance, pw::stream::Writer *aWriter)
    : mHdlcChannelOutput(*aWriter, pw::hdlc::kDefaultRpcAddress, "HDLC output")
    , mChannels{pw::rpc::Channel::Create<1>(reinterpret_cast<pw::rpc::ChannelOutput *>(&mHdlcChannelOutput))}
    , mServer(mChannels)
    , mNcpService(NcpService(aInstance))
    , mDecoder(mDecodeBuffer)
{
    mServer.RegisterService(mNcpService);
}

void RpcServer::ProcessPackets(const uint8_t *aBuffer, uint16_t aLength)
{
    for (uint16_t i = 0; i < aLength; i++)
    {
        std::byte data = (std::byte)aBuffer[i];

        if (auto result = mDecoder.Process(data); result.ok())
        {
            pw::hdlc::Frame &frame = result.value();

            if (frame.address() == pw::hdlc::kDefaultRpcAddress)
            {
                mServer.ProcessPacket(frame.data(), mHdlcChannelOutput);
            }
        }
    }
}

} // namespace Rpc
} // namespace ot

#include <openthread/ncp.h>
ot::Rpc::UartWriter sUartWriter;
ot::Rpc::RpcServer *sServer;

static OT_DEFINE_ALIGNED_VAR(sRpcServerVar, sizeof(ot::Rpc::RpcServer), uint64_t);

extern "C" void otNcpHdlcInit(otInstance *aInstance, otNcpHdlcSendCallback aSendCallback)
{
    sUartWriter.SetSendCallback(aSendCallback);
    sServer = new (&sRpcServerVar) ot::Rpc::RpcServer(aInstance, &sUartWriter);

    if (sServer == nullptr)
    {
        OT_ASSERT(false);
    }
}

extern "C" void otNcpHdlcSendDone(void)
{
}

extern "C" void otNcpHdlcReceive(const uint8_t *aBuf, uint16_t aBufLength)
{
    sServer->ProcessPackets(aBuf, aBufLength);
}
