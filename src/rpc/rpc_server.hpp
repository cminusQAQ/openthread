#include "ncp_service.hpp"
#include "rpc_channeloutput.hpp"
#include "syslog.h"
#include <openthread/instance.h>
#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/client.h"
#include "pw_rpc/server.h"
#include "pw_status/status.h"
#include "pw_stream/sys_io_stream.h"
#include "pw_sys_io/sys_io.h"

namespace ot {
namespace Rpc {

typedef int (*otNcpHdlcSendCallback)(const uint8_t *aBuf, uint16_t aBufLength);

class UartWriter : public pw::stream::NonSeekableWriter
{
public:
    void SetSendCallback(otNcpHdlcSendCallback aCallback) { mCallback = aCallback; }

private:
    pw::Status DoWrite(std::span<const std::byte> data) override;

    otNcpHdlcSendCallback mCallback;
};

class RpcServer
{
public:
    RpcServer(otInstance *aInstance, pw::stream::Writer *aWriter);

    void ProcessPackets(const uint8_t *aBuffer, uint16_t aLength);

    // private:
    RpcChannelOutput                                              mHdlcChannelOutput;
    pw::rpc::Channel                                              mChannels[1];
    pw::rpc::Server                                               mServer;
    NcpService                                                    mNcpService;
    pw::hdlc::Decoder                                             mDecoder;
    std::array<std::byte, RpcChannelOutput::kMaxTransmissionUnit> mDecodeBuffer;
};

} // namespace Rpc
} // namespace ot
