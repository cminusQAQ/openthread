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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "rpc/ncp_cli.hpp"

constexpr size_t          kMaxFrameSize = 8192;
int                       OutputCallback(void *aContext, const char *aFormat, va_list aArguments);
void                      UartCallback(uint8_t *aBuffer, uint16_t aLength);
static ot::Posix::Uart    sUart(UartCallback);
static ot::Rpc::RpcClient sRpcClient(&sUart);
static ot::Rpc::NcpCli    sCli(&sRpcClient, OutputCallback);

void UartCallback(uint8_t *aBuffer, const uint16_t aLength)
{
    sRpcClient.ProcessPackets(aBuffer, aLength);
}
int OutputCallback(void *aContext, const char *aFormat, va_list aArguments)
{
    OT_UNUSED_VARIABLE(aContext);
    return vdprintf(STDOUT_FILENO, aFormat, aArguments);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Invalid command!\nAdd the argument like "
               "'spinel+hdlc+forkpty://build/simulation/examples/apps/ncp/ot-ncp-ftd-rpc?forkpty-arg=1'\n");
        exit(EXIT_FAILURE);
    }
    ot::Posix::UartUrl url(argv[1]);

    sUart.Init(url);
    printf("> ");
    fflush(stdout);

    while (true)
    {
        int    maxfd = 4;
        fd_set rdfds;
        int    ret;

        FD_ZERO(&rdfds);
        FD_SET(STDIN_FILENO, &rdfds);
        sUart.UpdateFdSet(rdfds, maxfd);
        ret = select(maxfd, &rdfds, NULL, NULL, NULL);

        if (ret == -1)
        {
            printf("Fail to select!");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &rdfds))
        {
            char     szBuf[2000] = {0};
            uint16_t length      = read(STDIN_FILENO, szBuf, sizeof(szBuf) - 1);

            if (length == 0)
                exit(EXIT_SUCCESS);
            sCli.ProcessLine(szBuf);
            printf("> ");
            fflush(stdout);
        }
        sUart.Process(&rdfds);
    }
}
