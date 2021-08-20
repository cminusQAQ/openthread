#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "rpc/ncp_cli.hpp"

int OutputCallback(void *aContext, const char *aFormat, va_list aArguments)
{
    OT_UNUSED_VARIABLE(aContext);
    return vdprintf(STDOUT_FILENO, aFormat, aArguments);
}

constexpr size_t          kMaxFrameSize = 8192;
void                      UartCallback(uint8_t *aBuffer, uint16_t aLength);
static ot::Posix::Uart    sUart(UartCallback);
static ot::Rpc::RpcClient sRpcClient(&sUart);
static ot::Rpc::NcpCli    sCli(&sUart, &sRpcClient, OutputCallback);
void                      UartCallback(uint8_t *aBuffer, const uint16_t aLength)
{
    sRpcClient.ProcessPacket(aBuffer, aLength);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Invalid command, please input enough argument.\n");
        exit(1);
    }
    ot::Posix::UartUrl url(argv[1]);

    sUart.Init(url);
    printf("> ");
    fflush(stdout);
    while (true)
    {
        int            maxfd = 4;
        struct timeval tv;
        tv.tv_sec  = 1500;
        tv.tv_usec = 5000;
        fd_set rdfds;
        FD_ZERO(&rdfds);
        FD_SET(STDIN_FILENO, &rdfds);
        sUart.UpdateFdSet(rdfds, maxfd);
        int iRet = select(maxfd, &rdfds, NULL, NULL, &tv);
        if (iRet == -1)
        {
            printf("Fail to select!");
            break;
        }
        else if (iRet == 0)
        {
            printf("Timeout\n");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &rdfds))
        {
            char szBuf[2000] = {0};
            read(STDIN_FILENO, szBuf, sizeof(szBuf) - 1);
            sCli.ProcessLine(szBuf);
            printf("> ");
            fflush(stdout);
        }
        sUart.Process(&rdfds);
    }
}