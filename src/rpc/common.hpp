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

#include "pw_sys_io/sys_io.h"

namespace pw::sys_io {

/**
 * Fill a byte std::span from the sys io backend using ReadByte().
 *
 * This function is implemented by this facade and simply uses ReadByte() to
 * read enough bytes to fill the destination span. If there's an error reading a
 * byte, the read is aborted and the contents of the destination span are
 * undefined. This function blocks until either an error occurs, or all bytes
 * are successfully read from the backend's ReadByte() implementation.
 *
 * @param[in] dest  A byte std::span.
 *
 * @returns Return status is OkStatus() if the destination span was successfully
 *          filled. In all cases, the number of bytes successuflly read to the
 *          destination span are returned as part of the StatusWithSize.
 */
StatusWithSize ReadBytes(std::span<std::byte> dest)
{
    return StatusWithSize(dest.size_bytes());
}

/**
 * Write std::span of bytes out the sys io backend using WriteByte().
 *
 * This function is implemented by this facade and simply writes the source
 * contents using WriteByte(). If an error writing a byte is encountered, the
 * write is aborted and the error status returned. This function blocks until
 * either an error occurs, or all bytes are successfully read from the backend's
 * WriteByte() implementation.
 *
 * @param[in] src  A byte std::span.
 *
 * @returns Return status is OkStatus() if all the bytes from the source span were
 *          successfully written. In all cases, the number of bytes successfully written
 *          are returned as part of the StatusWithSize.
 */
StatusWithSize WriteBytes(std::span<const std::byte> src)
{
    return StatusWithSize(src.size_bytes());
}

} // namespace pw::sys_io
