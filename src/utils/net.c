#include "dynamic_libs/socket_functions.h"

int recvwait(int sock, unsigned char *buffer, int len)
{
    int recvBytes = 0;

    while(len)
    {
        int ret = recv(sock, buffer, len, 0);
        if(ret <= 0) {
            return ret;
        }

        len -= ret;
        buffer += ret;
        recvBytes += ret;
    }

    return recvBytes;
}
