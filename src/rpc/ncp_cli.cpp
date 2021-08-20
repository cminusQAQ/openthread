#include "ncp_cli.hpp"
#include "pw_string/to_string.h"

namespace ot {
namespace Rpc {

NcpCli::NcpCli(ot::Posix::Uart *aUart, RpcClient *aRpcClient, otCliOutputCallback aOutputCallback)
    : mRpcClient(aRpcClient)
    , mUart(aUart)
    , mOutputCallback(aOutputCallback)
{
}

const char *RoleToString(uint32_t aRole)
{
    static const char *const kRoleStrings[] = {
        "disabled", // (0) kRoleDisabled
        "detached", // (1) kRoleDetached
        "child",    // (2) kRoleChild
        "router",   // (3) kRoleRouter
        "leader",   // (4) kRoleLeader
    };
    return (aRole <= 4) ? kRoleStrings[aRole] : "invalid";
}

otError NcpCli::ProcessStatus(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    ot_Rpc_Empty request;
    if (!aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = OT_ERROR_INVALID_COMMAND);
    }
    mStatusCall = nanopb::NcpServiceClient::Status(
        mRpcClient->mChannels[0], request, [this](const ot_Rpc_StatusResponse &response, pw::Status status) {
            if (status.ok())
            {
                OutputLine("----------Received Status response: -----------");
                OutputLine("Channel: %2d", response.mChannel);
                OutputLine("DeviceRole: %s", RoleToString(response.mDeviceRole));
                OutputLine("PanId: 0x%04x", response.mPanId);
                OutputLine("ExtPanId: 0x%016lx", response.mExtPanId);
                OutputLine("ExtendedAddress: 0x%016lx", response.mExtendedAddress);
                OutputLine("Rloc16: 0x%04x", response.mRloc16);
            }
            else
            {
                OutputLine("Status failed with status %d", static_cast<int>(status.code()));
            }
        });

exit:
    return error;
}

otError NcpCli::ProcessScan(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    ot_Rpc_Empty request;
    if (!aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = OT_ERROR_INVALID_COMMAND);
    }
    OutputLine("| J | Network Name     | Extended PAN     | PAN  | MAC Address      | Ch | dBm | LQI "
               "|\n+---+------------------+------------------+------+------------------+----+-----+-----+");
    mScanCall = nanopb::NcpServiceClient::Scan(
        mRpcClient->mChannels[0], request,
        [this](const ot_Rpc_ScanResponse &response) {
            OutputFormat("| %d ", response.mIsJoinable);
            OutputFormat("| %-16s ", response.mNetworkName.bytes);
            OutputFormat("| %016lx ", response.mExtendedPanId);
            OutputFormat("| %04x ", response.mPanId);
            OutputFormat("| %016lx ", response.mExtAddress);
            OutputFormat("| %2d ", response.mChannel);
            OutputFormat("| %3d ", response.mRssi);
            OutputLine("| %3d |", response.mLqi);
        },
        [this](pw::Status status) {
            if (status.ok())
                ;
            else
                OutputLine("Scan failed with status %d", static_cast<int>(status.code()));
        });

exit:
    return error;
}

otError NcpCli::ProcessAttach(Arg aArgs[])
{
    otError        error = OT_ERROR_NONE;
    ot_Rpc_Dataset request;
    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = OT_ERROR_INVALID_COMMAND);
    }
    aArgs[0].ParseAsHexString(request.mTlvs.size = aArgs[0].GetLength(), request.mTlvs.bytes);
    mAttachCall = nanopb::NcpServiceClient::Attach(
        mRpcClient->mChannels[0], request, [this](const ot_Rpc_Empty, pw::Status status) {
            if (status.ok())
                ;
            else
            {
                OutputLine("Status failed with status %d\n", static_cast<int>(status.code()));
            }
        });
exit:
    return error;
}

otError NcpCli::ProcessDetach(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    ot_Rpc_Empty request;
    if (!aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = OT_ERROR_INVALID_COMMAND);
    }
    mDetachCall = nanopb::NcpServiceClient::Detach(
        mRpcClient->mChannels[0], request, [this](const ot_Rpc_Empty, pw::Status status) {
            if (status.ok())
                ;
            else
            {
                OutputLine("Status failed with status %d\n", static_cast<int>(status.code()));
            }
        });
exit:
    return error;
}

int NcpCli::OutputFormat(const char *aFormat, ...)
{
    int     rval;
    va_list ap;

    va_start(ap, aFormat);
    rval = OutputFormatV(aFormat, ap);
    va_end(ap);

    return rval;
}

void NcpCli::OutputLine(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    OutputFormatV(aFormat, args);
    va_end(args);

    OutputFormat("\r\n");
}

int NcpCli::OutputFormatV(const char *aFormat, va_list aArguments)
{
    return mOutputCallback(mOutputContext, aFormat, aArguments);
}

void NcpCli::OutputResult(otError aError)
{
    switch (aError)
    {
    case OT_ERROR_NONE:
        OutputLine("Done");
        break;

    case OT_ERROR_PENDING:
        break;

    default:
        OutputLine("Error %d: %s\n", aError, otThreadErrorToString(aError));
    }
}

void NcpCli::ProcessLine(char *aBuf)
{
    Arg            args[kMaxArgs + 1];
    const Command *command;
    otError        error = OT_ERROR_NONE;

    OT_ASSERT(aBuf != nullptr);

    VerifyOrExit(StringLength(aBuf, kMaxLineLength) <= kMaxLineLength - 1, error = OT_ERROR_PARSE);

    error = Utils::CmdLineParser::ParseCmd(aBuf, args);

    if (error != OT_ERROR_NONE)
    {
        OutputLine("Error: too many args (max %d)", kMaxArgs);
        ExitNow();
    }

    VerifyOrExit(!args[0].IsEmpty());

    command = Utils::LookupTable::Find(args[0].GetCString(), sCommands);
    if (command != nullptr)
    {
        error = (this->*command->mHandler)(args + 1);
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }
exit:
    if ((error != OT_ERROR_NONE) || !args[0].IsEmpty())
    {
        OutputResult(error);
    }
}
} // namespace Rpc
} // namespace ot
