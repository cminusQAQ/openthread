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

#include "common/instance.hpp"
#include "proto/ncp.rpc.pb.h"

namespace ot::Rpc {

class NcpService : public generated::NcpService<NcpService>
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in] aInstance  The openthread instance structure.
     *
     */
    NcpService(otInstance *aInstance)
        : mInstance(aInstance)
    {
    }

    /**
     * This method process RPC service 'status'.
     *
     * @param[in] request   The RPC request.
     * @param[in] response  The RPC response.
     *
     * @retval OkStatus()  Successfully get the RPC response.
     *
     */
    ::pw::Status Status(ServerContext &, const ot_Rpc_Empty &request, ot_Rpc_StatusResponse &response);

    /**
     * This method process RPC service 'scan'.
     *
     * @param[in] request  The RPC request.
     * @param[in] writer   A server writer to send a response when a new device is detected.
     *
     */
    void Scan(ServerContext &, const ot_Rpc_Empty &request, ServerWriter<ot_Rpc_ScanResponse> &writer);

    /**
     * This method process RPC service 'attach'.
     *
     * @param[in] request   The RPC request.
     *
     * @retval OkStatus()                  Successfully get the RPC response.
     * @retval PW_STATUS_INVALID_ARGUMENT  There are invaild arguments in the request.
     * @retval PW_STATUS_UNKNOWN           Unknow error.
     *
     */
    ::pw::Status Attach(ServerContext &, const ot_Rpc_Dataset &request, ot_Rpc_Empty &);

    /**
     * This method process RPC service 'attach'.
     *
     * @param[in] request   The RPC request.
     *
     * @retval OkStatus()                  Successfully get the RPC response.
     * @retval PW_STATUS_UNKNOWN           Unknow error.
     *
     */
    ::pw::Status Detach(ServerContext &, const ot_Rpc_Empty &, ot_Rpc_Empty &);

private:
    static uint64_t                   ConvertOpenThreadUint64(const uint8_t *aValue);
    static void                       LinkActiveScanDone(otActiveScanResult *aResult, void *aContext);
    ::pw::Status                      ErrorToStatus(otError error);
    ServerWriter<ot_Rpc_ScanResponse> sWriter;
    otInstance *                      mInstance;
};

} // namespace ot::Rpc
