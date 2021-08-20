#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/client.h"

#include "rpc_channeloutput.hpp"
#include "pw_rpc/channel.h"
#include "pw_status/status.h"
#include "pw_stream/sys_io_stream.h"

namespace ot {
namespace Rpc {

class RpcClient
{
public:
    RpcClient(pw::stream::Writer *aWriter);

    // Called when the transport layer receives an RPC packet.
    void ProcessPacket(const uint8_t *aBuffer, uint16_t aLength);

    RpcChannelOutput                                              mChannelOutput;
    pw::rpc::Channel                                              mChannels[1];
    pw::rpc::Client                                               mClient;
    pw::hdlc::Decoder                                             mDecoder;
    std::array<std::byte, RpcChannelOutput::kMaxTransmissionUnit> mDecodeBuffer;
};
} // namespace Rpc
} // namespace ot
