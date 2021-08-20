#include "uart.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/error.hpp"
#include "common/string.hpp"
#include "common/type_traits.hpp"
#include "proto/ncp.rpc.pb.h"
#include "rpc/rpc_client.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {

namespace Rpc {

class NcpCli
{
public:
    typedef Utils::CmdLineParser::Arg Arg;
    typedef int (*otCliOutputCallback)(void *aContext, const char *aFormat, va_list aArguments);

    NcpCli(ot::Posix::Uart *aUart, RpcClient *aRpcClient, otCliOutputCallback aOutputCallback);

    void ProcessLine(char *aBuf);

    int OutputFormat(const char *aFormat, ...);

    void OutputLine(const char *aFormat, ...);

    int OutputFormatV(const char *aFormat, va_list aArguments);

    void OutputResult(otError aError);

    otError ProcessStatus(Arg aArgs[]);

    otError ProcessScan(Arg aArgs[]);

    otError ProcessAttach(Arg aArgs[]);

    otError ProcessDetach(Arg aArgs[]);

private:
    enum
    {
        kIndentSize       = 4,
        kMaxArgs          = 32,
        kMaxAutoAddresses = 8,
        kMaxLineLength    = OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH,
    };

    struct Command
    {
        const char *mName;
        otError (NcpCli::*mHandler)(Arg aArgs[]);
    };
    void *                               mOutputContext;
    RpcClient *                          mRpcClient;
    ot::Posix::Uart *                    mUart;
    otCliOutputCallback                  mOutputCallback;
    nanopb::NcpServiceClient::StatusCall mStatusCall;
    nanopb::NcpServiceClient::ScanCall   mScanCall;
    nanopb::NcpServiceClient::AttachCall mAttachCall;
    nanopb::NcpServiceClient::DetachCall mDetachCall;

    static constexpr Command sCommands[] = {
        {"attach", &NcpCli::ProcessAttach},
        {"detach", &NcpCli::ProcessDetach},
        {"scan", &NcpCli::ProcessScan},
        {"status", &NcpCli::ProcessStatus},
    };
};
} // namespace Rpc

} // namespace ot
