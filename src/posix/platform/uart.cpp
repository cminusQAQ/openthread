/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
#include "uart.hpp"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"

//#define OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE 1

namespace ot {
namespace Posix {

UartUrl::UartUrl(const char *aUrl)
{
    VerifyOrDie(strnlen(aUrl, sizeof(mUrl)) < sizeof(mUrl), OT_EXIT_INVALID_ARGUMENTS);
    strncpy(mUrl, aUrl, sizeof(mUrl) - 1);
    SuccessOrDie(Url::Url::Init(mUrl));
}

Uart::Uart(UartCallback aCallback)
    : mSockFd(-1)
    , mUartCallback(aCallback)
{
}

Uart::~Uart()
{
    Deinit();
}

otError Uart::SendFrame(const uint8_t *aFrame, uint16_t aLength)
{
    otError error = Write(aFrame, aLength);
    return error;
}

void Uart::Deinit(void)
{
    CloseFile();
}

void Uart::CloseFile(void)
{
    VerifyOrExit(mSockFd != -1);

    VerifyOrExit(0 == close(mSockFd), perror("close RCP"));
    VerifyOrExit(-1 != wait(nullptr) || errno == ECHILD, perror("wait RCP"));

    mSockFd = -1;

exit:
    return;
}

void Uart::Read(void)
{
    uint8_t buffer[kMaxFrameSize] = {0};
    ssize_t rval;
    rval = read(mSockFd, buffer, sizeof(buffer));

    if (rval > 0)
    {
        mUartCallback(buffer, static_cast<uint16_t>(rval));
    }
    else if ((rval < 0) && (errno != EAGAIN) && (errno != EINTR))
    {
        DieNow(OT_EXIT_ERROR_ERRNO);
    }
}

int Uart::OpenFile(const Url::Url &aUartUrl)
{
    int fd   = -1;
    int rval = 0;

    fd = open(aUartUrl.GetPath(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_CLOEXEC);
    if (fd == -1)
    {
        perror("open uart failed");
        ExitNow();
    }

    if (isatty(fd))
    {
        struct termios tios;
        const char *   value;
        speed_t        speed;

        int      stopBit  = 1;
        uint32_t baudrate = 115200;

        VerifyOrExit((rval = tcgetattr(fd, &tios)) == 0);

        cfmakeraw(&tios);

        tios.c_cflag = CS8 | HUPCL | CREAD | CLOCAL;

        if ((value = aUartUrl.GetValue("uart-parity")) != nullptr)
        {
            if (strncmp(value, "odd", 3) == 0)
            {
                tios.c_cflag |= PARENB;
                tios.c_cflag |= PARODD;
            }
            else if (strncmp(value, "even", 4) == 0)
            {
                tios.c_cflag |= PARENB;
            }
            else
            {
                DieNow(OT_EXIT_INVALID_ARGUMENTS);
            }
        }

        if ((value = aUartUrl.GetValue("uart-stop")) != nullptr)
        {
            stopBit = atoi(value);
        }

        switch (stopBit)
        {
        case 1:
            tios.c_cflag &= static_cast<unsigned long>(~CSTOPB);
            break;
        case 2:
            tios.c_cflag |= CSTOPB;
            break;
        default:
            DieNow(OT_EXIT_INVALID_ARGUMENTS);
            break;
        }

        if ((value = aUartUrl.GetValue("uart-baudrate")))
        {
            baudrate = static_cast<uint32_t>(atoi(value));
        }

        switch (baudrate)
        {
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
#ifdef B230400
        case 230400:
            speed = B230400;
            break;
#endif
#ifdef B460800
        case 460800:
            speed = B460800;
            break;
#endif
#ifdef B500000
        case 500000:
            speed = B500000;
            break;
#endif
#ifdef B576000
        case 576000:
            speed = B576000;
            break;
#endif
#ifdef B921600
        case 921600:
            speed = B921600;
            break;
#endif
#ifdef B1000000
        case 1000000:
            speed = B1000000;
            break;
#endif
#ifdef B1152000
        case 1152000:
            speed = B1152000;
            break;
#endif
#ifdef B1500000
        case 1500000:
            speed = B1500000;
            break;
#endif
#ifdef B2000000
        case 2000000:
            speed = B2000000;
            break;
#endif
#ifdef B2500000
        case 2500000:
            speed = B2500000;
            break;
#endif
#ifdef B3000000
        case 3000000:
            speed = B3000000;
            break;
#endif
#ifdef B3500000
        case 3500000:
            speed = B3500000;
            break;
#endif
#ifdef B4000000
        case 4000000:
            speed = B4000000;
            break;
#endif
        default:
            DieNow(OT_EXIT_INVALID_ARGUMENTS);
            break;
        }

        mBaudRate = baudrate;

        if (aUartUrl.GetValue("uart-flow-control") != nullptr)
        {
            tios.c_cflag |= CRTSCTS;
        }

        VerifyOrExit((rval = cfsetspeed(&tios, static_cast<speed_t>(speed))) == 0, perror("cfsetspeed"));
        VerifyOrExit((rval = tcsetattr(fd, TCSANOW, &tios)) == 0, perror("tcsetattr"));
        VerifyOrExit((rval = tcflush(fd, TCIOFLUSH)) == 0);
    }

exit:
    if (rval != 0)
    {
        DieNow(OT_EXIT_FAILURE);
    }

    return fd;
}

#if OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
int Uart::ForkPty(const Url::Url &aUartUrl)
{
    int fd   = -1;
    int pid  = -1;
    int rval = -1;

    {
        struct termios tios;

        memset(&tios, 0, sizeof(tios));
        cfmakeraw(&tios);
        tios.c_cflag = CS8 | HUPCL | CREAD | CLOCAL;

        VerifyOrDie((pid = forkpty(&fd, nullptr, &tios, nullptr)) != -1, OT_EXIT_ERROR_ERRNO);
    }

    if (0 == pid)
    {
        constexpr int kMaxArguments = 32;
        char *        argv[kMaxArguments + 1];
        size_t        index = 0;

        argv[index++] = const_cast<char *>(aUartUrl.GetPath());

        for (const char *arg = nullptr;
             index < OT_ARRAY_LENGTH(argv) && (arg = aUartUrl.GetValue("forkpty-arg", arg)) != nullptr;
             argv[index++] = const_cast<char *>(arg))
        {
        }

        if (index < OT_ARRAY_LENGTH(argv))
        {
            argv[index] = nullptr;
        }
        else
        {
            DieNowWithMessage("Too many arguments!", OT_EXIT_INVALID_ARGUMENTS);
        }

        VerifyOrDie((rval = execvp(argv[0], argv)) != -1, OT_EXIT_ERROR_ERRNO);
    }
    else
    {
        VerifyOrDie((rval = fcntl(fd, F_GETFL)) != -1, OT_EXIT_ERROR_ERRNO);
        VerifyOrDie((rval = fcntl(fd, F_SETFL, rval | O_NONBLOCK | O_CLOEXEC)) != -1, OT_EXIT_ERROR_ERRNO);
    }

    return fd;
}
#endif // OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE

otError Uart::Init(const Url::Url &aUartUrl)
{
    otError     error = OT_ERROR_NONE;
    struct stat st;
    VerifyOrExit(mSockFd == -1, error = OT_ERROR_ALREADY);
    VerifyOrDie(stat(aUartUrl.GetPath(), &st) == 0, OT_EXIT_INVALID_ARGUMENTS);
    if (S_ISCHR(st.st_mode))
    {
        mSockFd = OpenFile(aUartUrl);
        VerifyOrExit(mSockFd != -1, error = OT_ERROR_INVALID_ARGS);
    }
#if OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
    else if (S_ISREG(st.st_mode))
    {
        mSockFd = ForkPty(aUartUrl);
        VerifyOrExit(mSockFd != -1, error = OT_ERROR_INVALID_ARGS);
    }
#endif // OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
    else
    {
        otLogCritPlat("Uart file '%s' not supported", aUartUrl.GetPath());
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

    mUartUrl = &aUartUrl;

exit:
    return error;
}

void Uart::UpdateFdSet(fd_set &aReadFdSet, int &aMaxFd)
{
    FD_SET(mSockFd, &aReadFdSet);

    if (aMaxFd < mSockFd)
    {
        aMaxFd = mSockFd;
    }
}

void Uart::Process(fd_set *aReadFdSet)
{
    if (FD_ISSET(mSockFd, aReadFdSet))
    {
        Read();
    }
}

pw::Status Uart::DoWrite(std::span<const std::byte> data)
{
    uint8_t Frame[kMaxFrameSize];
    for (uint16_t i = 0; i < data.size(); i++) Frame[i] = (uint8_t)data[i];
    SendFrame(Frame, data.size());
    return pw::OkStatus();
}

otError Uart::Write(const uint8_t *aFrame, uint16_t aLength)
{
    otError error = OT_ERROR_NONE;
#if OPENTHREAD_POSIX_VIRTUAL_TIME
    virtualTimeSendUartSpinelWriteEvent(aFrame, aLength);
#else
    while (aLength)
    {
        ssize_t rval = write(mSockFd, aFrame, aLength);
        if (rval == aLength)
        {
            break;
        }
        else if (rval > 0)
        {
            aLength -= static_cast<uint16_t>(rval);
            aFrame += static_cast<uint16_t>(rval);
        }
        else if (rval < 0)
        {
            VerifyOrDie((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR), OT_EXIT_ERROR_ERRNO);
        }
    }

#endif // OPENTHREAD_POSIX_VIRTUAL_TIME
    return error;
}
} // namespace Posix
} // namespace ot

void _otLogCrit(otLogRegion aRegion, const char *aFormat, ...)
{
    (void)aRegion;
    (void)aFormat;
}
