/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "ncp_service.hpp"

namespace ot::Rpc {

uint64_t NcpService::ConvertOpenThreadUint64(const uint8_t *aValue)
{
    uint64_t val = 0;
    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        val = (val << 8) | aValue[i];
    }
    return val;
}

::pw::Status NcpService::Status(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_StatusResponse &response)
{
    const char *networkname = otThreadGetNetworkName(mInstance);

    response.mNetworkName.size = strlen(networkname);
    memcpy(response.mNetworkName.bytes, networkname, sizeof(char) * response.mNetworkName.size);

    response.mChannel         = otLinkGetChannel(mInstance);
    response.mDeviceRole      = otThreadGetDeviceRole(mInstance);
    response.mPanId           = otLinkGetPanId(mInstance);
    response.mExtPanId        = ConvertOpenThreadUint64(otThreadGetExtendedPanId(mInstance)->m8);
    response.mExtendedAddress = ConvertOpenThreadUint64(otLinkGetExtendedAddress(mInstance)->m8);
    response.mRloc16          = otThreadGetRloc16(mInstance);

    return pw::OkStatus();
}

void NcpService::Scan(ServerContext &, const ot_Rpc_Empty &, ServerWriter<ot_Rpc_ScanResponse> &writer)
{
    (void)otLinkActiveScan(mInstance, 0, 0, &LinkActiveScanDone, this);
    sWriter = std::move(writer);
}

void NcpService::LinkActiveScanDone(otActiveScanResult *aResult, void *aContext)
{
    ot_Rpc_ScanResponse response;

    if (aResult == nullptr)
    {
        static_cast<NcpService *>(aContext)->sWriter.Finish();
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
        static_cast<NcpService *>(aContext)->sWriter.Write(response);
    }
}

::pw::Status NcpService::ErrorToStatus(otError error)
{
    ::pw::Status status;
    switch (error)
    {
    case OT_ERROR_NONE:
        status = pw::OkStatus();
        break;
    case OT_ERROR_INVALID_ARGS:
        status = PW_STATUS_INVALID_ARGUMENT;
        break;
    default:
        status = PW_STATUS_UNKNOWN;
    }
    return status;
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
        SuccessOrExit(error = otDatasetSetActiveTlvs(mInstance, &datasetTlvs));
        if (!otIp6IsEnabled(mInstance))
        {
            SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
        }
        SuccessOrExit(error = otThreadSetEnabled(mInstance, true));
        ExitNow();
    }

exit:
    return ErrorToStatus(error);
}

::pw::Status NcpService::Detach(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_Empty &)
{
    otError error = OT_ERROR_NONE;
    SuccessOrExit(error = otThreadSetEnabled(mInstance, false));
    SuccessOrExit(error = otIp6SetEnabled(mInstance, false));
exit:
    return ErrorToStatus(error);
}

} // namespace ot::Rpc
