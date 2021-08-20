#include "rpc_server.hpp"
//#include "pw_rpc/echo_service_nanopb.h"
#include "common.hpp"
#include "syslog.h"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
namespace ot {
namespace Rpc {

pw::Status UartWriter::DoWrite(std::span<const std::byte> data)
{
    uint8_t  buf[RpcChannelOutput::kMaxTransmissionUnit] = {0};
    uint16_t size                                        = data.size();
    for (uint16_t i = 0; i < size; i++)
    {
        uint16_t pos = i % RpcChannelOutput::kMaxTransmissionUnit;
        buf[pos]     = (uint8_t)data[i];
        if (pos == RpcChannelOutput::kMaxTransmissionUnit - 1 || pos == size - 1)
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

#include <syslog.h>
#include <openthread/ncp.h>
ot::Rpc::UartWriter sUartWriter;
ot::Rpc::RpcServer *sServer;

static OT_DEFINE_ALIGNED_VAR(sRpcServerVar, sizeof(ot::Rpc::RpcServer), uint64_t);

// uint64_t sRpcServerVar[sizeof(ot::Rpc::RpcServer) / sizeof(uint16_t) + sizeof(uint64_t)];

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