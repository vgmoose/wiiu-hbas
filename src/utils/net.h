#ifndef __NET_H_
#define __NET_H_

#ifdef __cplusplus
extern "C" {
#endif

int recvwait(int sock, unsigned char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif
