#include "rpc_client.hpp"
#include "common.hpp"

namespace ot {
namespace Rpc {

RpcClient::RpcClient(pw::stream::Writer *aWriter)
    : mChannelOutput(*aWriter, pw::hdlc::kDefaultRpcAddress, "HDLC output")
    , mChannels{pw::rpc::Channel::Create<1>(&mChannelOutput)}
    , mClient(mChannels)
    , mDecoder(mDecodeBuffer)
{
}

void RpcClient::ProcessPacket(const uint8_t *aBuffer, uint16_t aLength)
{
    for (uint16_t i = 0; i < aLength; i++)
    {
        std::byte data = (std::byte)aBuffer[i];
        if (auto result = mDecoder.Process(data); result.ok())
        {
            pw::hdlc::Frame &frame = result.value();
            if (frame.address() == pw::hdlc::kDefaultRpcAddress)
            {
                mClient.ProcessPacket(frame.data());
            }
        }
    }
}

} // namespace Rpc
} // namespace ot
