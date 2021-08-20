#include <syslog.h>
#include "common/instance.hpp"
#include "proto/ncp.rpc.pb.h"

namespace ot::Rpc {

// Implementation class for ot.Rpc.NcpService.
class NcpService : public generated::NcpService<NcpService>
{
public:
    NcpService(otInstance *aInstance)
        : mInstance(aInstance)
    {
    }

    ::pw::Status Status(ServerContext &, const ot_Rpc_Empty &request, ot_Rpc_StatusResponse &response);

    void Scan(ServerContext &, const ot_Rpc_Empty &request, ServerWriter<ot_Rpc_ScanResponse> &writer);

    ::pw::Status Attach(ServerContext &, const ot_Rpc_Dataset &request, ot_Rpc_Empty &);

    ::pw::Status Detach(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_Empty &);

private:
    otInstance *mInstance;
};

} // namespace ot::Rpc
