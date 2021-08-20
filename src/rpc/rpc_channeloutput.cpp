#include "rpc_channeloutput.hpp"
#include <syslog.h>

namespace ot {
namespace Rpc {

RpcChannelOutput::RpcChannelOutput(pw::stream::Writer &writer, uint8_t address, const char *channel_name)
    : pw::rpc::ChannelOutput(channel_name)
    , mWriter(writer)
    , mAddress(address)
{
}

std::span<std::byte> RpcChannelOutput::AcquireBuffer()
{
    return mBuffer;
}

pw::Status RpcChannelOutput::SendAndReleaseBuffer(std::span<const std::byte> buffer)
{
    pw::Status ret;
    PW_DASSERT(buffer.data() == mBuffer.data());
    if (buffer.empty())
    {
        ret = pw::OkStatus();
    }
    else
    {
        ret = pw::hdlc::WriteUIFrame(mAddress, buffer, mWriter);
    }
    return ret;
}
} // namespace Rpc
} // namespace ot
