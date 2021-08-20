#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/client.h"

#include "pw_rpc/channel.h"
#include "pw_status/status.h"
#include "pw_stream/sys_io_stream.h"

namespace ot {
namespace Rpc {
class RpcChannelOutput : public pw::rpc::ChannelOutput
{
public:
    RpcChannelOutput(pw::stream::Writer &writer, uint8_t address, const char *channel_name);

    std::span<std::byte> AcquireBuffer() override;

    pw::Status          SendAndReleaseBuffer(std::span<const std::byte> buffer) override;
    static const size_t kMaxTransmissionUnit = 1500;

private:
    pw::stream::Writer &                        mWriter;
    std::array<std::byte, kMaxTransmissionUnit> mBuffer;
    const uint8_t                               mAddress;
};
} // namespace Rpc
} // namespace ot
