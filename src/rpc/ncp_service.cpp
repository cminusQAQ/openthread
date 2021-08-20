#include "ncp_service.hpp"
#include <random>
#include <syslog.h>
static uint64_t ConvertOpenThreadUint64(const uint8_t *aValue)
{
    uint64_t val = 0;
    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        val = (val << 8) | aValue[i];
    }
    return val;
}

namespace ot::Rpc {

::pw::Status NcpService::Status(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_StatusResponse &response)
{
    response.mChannel         = otLinkGetChannel(mInstance);
    response.mDeviceRole      = otThreadGetDeviceRole(mInstance);
    response.mPanId           = otLinkGetPanId(mInstance);
    response.mExtPanId        = ConvertOpenThreadUint64(otThreadGetExtendedPanId(mInstance)->m8);
    response.mExtendedAddress = ConvertOpenThreadUint64(otLinkGetExtendedAddress(mInstance)->m8);
    response.mRloc16          = otThreadGetRloc16(mInstance);

    return pw::OkStatus();
}

static NcpService::ServerWriter<ot_Rpc_ScanResponse> sWriter;
void                                                 LinkActiveScanDone(otActiveScanResult *aResult, void *);

void NcpService::Scan(ServerContext &, const ot_Rpc_Empty &, ServerWriter<ot_Rpc_ScanResponse> &writer)
{
    otLinkActiveScan(mInstance, 0, 0, &LinkActiveScanDone, this);
    sWriter = std::move(writer);
}

void LinkActiveScanDone(otActiveScanResult *aResult, void *)
{
    ot_Rpc_ScanResponse response;
    if (aResult == nullptr)
    {
        sWriter.Finish();
    }
    else
    {
        response.mExtAddress = ConvertOpenThreadUint64((uint8_t *)(aResult->mExtAddress.m8));
        memcpy(response.mNetworkName.bytes, aResult->mNetworkName.m8, sizeof(aResult->mNetworkName.m8));
        response.mNetworkName.size = strlen(aResult->mNetworkName.m8);
        response.mExtendedPanId    = ConvertOpenThreadUint64((uint8_t *)(aResult->mExtendedPanId.m8));
        memcpy(response.mSteeringData.bytes, aResult->mSteeringData.m8, sizeof(aResult->mSteeringData.m8));
        response.mSteeringData.size = aResult->mSteeringData.mLength;
        response.mPanId             = aResult->mPanId;
        response.mJoinerUdpPort     = aResult->mJoinerUdpPort;
        response.mChannel           = aResult->mChannel;
        response.mRssi              = aResult->mRssi;
        response.mLqi               = aResult->mLqi;
        response.mVersion           = aResult->mVersion;
        response.mIsNative          = aResult->mIsNative;
        response.mIsJoinable        = aResult->mIsJoinable;
        sWriter.Write(response);
    }
}

::pw::Status NcpService::Attach(ServerContext &, const ot_Rpc_Dataset &request, ot_Rpc_Empty &)
{
    otError                  error = OT_ERROR_NONE;
    otOperationalDatasetTlvs datasetTlvs;
    otOperationalDataset     dataset;
    otDeviceRole             role = otThreadGetDeviceRole(mInstance);

    VerifyOrExit(request.mTlvs.size <= sizeof(datasetTlvs.mTlvs), error = OT_ERROR_INVALID_ARGS);
    std::copy(request.mTlvs.bytes, request.mTlvs.bytes + request.mTlvs.size, datasetTlvs.mTlvs);
    datasetTlvs.mLength = request.mTlvs.size;
    SuccessOrExit(error = otDatasetParseTlvs(&datasetTlvs, &dataset));
    VerifyOrExit(dataset.mComponents.mIsActiveTimestampPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsNetworkKeyPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsNetworkNamePresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsExtendedPanIdPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsMeshLocalPrefixPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsPanIdPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsChannelPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsPskcPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsSecurityPolicyPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsChannelMaskPresent, error = OT_ERROR_INVALID_ARGS);
    if (role == OT_DEVICE_ROLE_DISABLED || role == OT_DEVICE_ROLE_DETACHED)
    {
        // otOperationalDataset existingDataset;
        // error = otDatasetGetActive(mInstance, &existingDataset);
        //  VerifyOrExit(error == OT_ERROR_NONE || error == OT_ERROR_NOT_FOUND);
        //   VerifyOrExit(error == OT_ERROR_NOT_FOUND, error = OT_ERROR_INVALID_STATE);
        SuccessOrExit(error = otDatasetSetActiveTlvs(mInstance, &datasetTlvs));
        if (!otIp6IsEnabled(mInstance))
        {
            SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
        }
        SuccessOrExit(error = otThreadSetEnabled(mInstance, true));
        ExitNow();
    }

exit:
    switch (error)
    {
    case OT_ERROR_NONE:
        return pw::OkStatus();
    case OT_ERROR_INVALID_ARGS:
        return PW_STATUS_INVALID_ARGUMENT;
    default:
        return PW_STATUS_UNKNOWN;
    }
}

::pw::Status NcpService::Detach(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_Empty &)
{
    otError error = OT_ERROR_NONE;
    SuccessOrExit(error = otThreadSetEnabled(mInstance, false));
    SuccessOrExit(error = otIp6SetEnabled(mInstance, false));
exit:
    if (error == OT_ERROR_NONE)
        return pw::OkStatus();
    else
        return PW_STATUS_UNKNOWN;
}
} // namespace ot::Rpc
