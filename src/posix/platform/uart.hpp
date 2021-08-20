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

#include "openthread-posix-config.h"

#include <errno.h>
#include <net/if.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/openthread-system.h>
#include <openthread/platform/time.h>

#include "common/logging.hpp"

#include "lib/platform/exit_code.h"
#include "lib/url/url.hpp"
#include "pw_stream/stream.h"

namespace ot {
namespace Posix {

class UartUrl : public ot::Url::Url
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]   aUrl    The null-terminated URL string.
     *
     */
    UartUrl(const char *aUrl);

private:
    enum
    {
        kUartUrlMaxSize = 512,
    };
    char mUrl[kUartUrlMaxSize];
};

class Uart : public pw::stream::NonSeekableWriter
{
public:
    struct UartProcessContext
    {
        const fd_set *mReadFdSet;
        const fd_set *mWriteFdSet;
    };
    typedef void (*UartCallback)(uint8_t *aBuffer, uint16_t aLength);

    /**
     * This constructor initializes the object.
     *
     * @param[in] aCallback         Callback on frame received
     *
     */
    Uart(UartCallback aCallback);

    /**
     * This destructor deinitializes the object.
     *
     */
    ~Uart(void);

    /**
     * This method sends a hdlc frame to NCP over the socket.
     *
     * @param[in] aFrame     A pointer to buffer containing the hdlc frame to send.
     * @param[in] aLength    The length (number of bytes) in the frame.
     *
     * @retval OT_ERROR_NONE     Successfully encoded and sent the hdlc frame.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to encode the frame.
     *
     */
    otError SendFrame(const uint8_t *aFrame, uint16_t aLength);

    /**
     * This method deinitializes the interface to the NCP.
     *
     */
    void Deinit(void);

    /**
     * This method initializes the interface to the NCP
     *
     * @note This method should be called before reading and sending hdlc frames to the interface.
     *
     * @param[in]  aUartUrl          RadioUrl parsed from radio url.
     *
     * @retval OT_ERROR_NONE          The interface is initialized successfully
     * @retval OT_ERROR_ALREADY       The interface is already initialized.
     * @retval OT_ERROR_INVALID_ARGS  The UART device or executable cannot be found or failed to open/run.
     *
     */
    otError Init(const Url::Url &aUartUrl);

    /**
     * This method updates the file descriptor sets with file descriptors used by the radio driver.
     *
     * @param[inout]  aReadFdSet   A reference to the read file descriptors.
     * @param[inout]  aMaxFd       A reference to the max file descriptor.
     *
     */
    void UpdateFdSet(fd_set &aReadFdSet, int &aMaxFd);

    /**
     * This method performs radio driver processing.
     *
     * @param[in]   aContext        The context containing fd_sets.
     *
     */
    void Process(fd_set *aReadFdSet);

private:
    enum
    {
        kMaxFrameSize = 8192,
    };

    int        OpenFile(const Url::Url &aUartUrl);
    void       CloseFile(void);
    int        ForkPty(const Url::Url &aUartUrl);
    void       HandleReceive(uint8_t *aBuffer, uint16_t aLength);
    pw::Status DoWrite(std::span<const std::byte> data) override;
    otError    Write(const uint8_t *aFrame, uint16_t aLength);
    void       Read(void);

    uint32_t        mBaudRate;
    int             mSockFd;
    UartCallback    mUartCallback;
    const Url::Url *mUartUrl;
};

} // namespace Posix
} // namespace ot
