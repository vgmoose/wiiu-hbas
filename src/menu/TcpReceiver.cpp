#include <algorithm>
#include <string>
#include <string.h>
#include <zlib.h>

#include "TcpReceiver.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "fs/CFile.hpp"
#include "utils/logger.h"
#include "utils/StringTools.h"
#include "utils/net.h"

TcpReceiver::TcpReceiver(int port)
    : GuiFrame(0, 0)
    , CThread(CThread::eAttributeAffCore0 | CThread::eAttributePinnedAff)
    , exitRequested(false)
    , loadAddress(0)
    , serverPort(port)
    , serverSocket(-1)
    , progressWindow("Receiving file...")
{
    width = progressWindow.getWidth();
    height = progressWindow.getHeight();
    append(&progressWindow);

    u32 ApplicationMemoryEnd;
    asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l" : "=r" (ApplicationMemoryEnd));

    loadAddress = (unsigned char*)ApplicationMemoryEnd;
    resumeThread();
}

TcpReceiver::~TcpReceiver()
{
    exitRequested = true;

    if(serverSocket > 0)
    {
        shutdown(serverSocket, SHUT_RDWR);
    }
}

void TcpReceiver::executeThread()
{
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (serverSocket < 0)
		return;

    u32 enable = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

	struct sockaddr_in bindAddress;
	memset(&bindAddress, 0, sizeof(bindAddress));
	bindAddress.sin_family = AF_INET;
	bindAddress.sin_port = serverPort;
	bindAddress.sin_addr.s_addr = INADDR_ANY;

	s32 ret;
	if ((ret = bind(serverSocket, (struct sockaddr *)&bindAddress, sizeof(bindAddress))) < 0) {
		socketclose(serverSocket);
		return;
	}

	if ((ret = listen(serverSocket, 3)) < 0) {
		socketclose(serverSocket);
		return;
	}

	struct sockaddr_in clientAddr;
	s32 addrlen = sizeof(struct sockaddr);

    while(!exitRequested)
    {
        s32 clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
        if(clientSocket >= 0)
        {
            u32 ipAddress = clientAddr.sin_addr.s_addr;
            serverReceiveStart(this, ipAddress);
            int result = loadToMemory(clientSocket, ipAddress);
            serverReceiveFinished(this, ipAddress, result);
            socketclose(clientSocket);
        }
        else
        {
            usleep(100000);
        }
    }

    socketclose(serverSocket);
}

int TcpReceiver::loadToMemory(s32 clientSocket, u32 ipAddress)
{
    log_printf("Loading file from ip %08X\n", ipAddress);

    u32 fileSize = 0;
    u32 fileSizeUnc = 0;
    unsigned char haxx[8];
    memset(haxx, 0, sizeof(haxx));
    //skip haxx
    recvwait(clientSocket, haxx, sizeof(haxx));
    recvwait(clientSocket, (unsigned char*)&fileSize, sizeof(fileSize));

    if (haxx[4] > 0 || haxx[5] > 4)
    {
        recvwait(clientSocket, (unsigned char*)&fileSizeUnc, sizeof(fileSizeUnc)); // Compressed protocol, read another 4 bytes
    }

    u32 bytesRead = 0;
    struct in_addr in;
    in.s_addr = ipAddress;
    progressWindow.setTitle(strfmt("Loading file from %s", inet_ntoa(in)));

    log_printf("transfer start\n");

    std::string strBuffer;
    strBuffer.resize(0x1000);

    // Copy rpl in memory
    while(bytesRead < fileSize)
    {
        progressWindow.setProgress(100.0f * (f32)bytesRead / (f32)fileSize);

        u32 blockSize = strBuffer.size();
        if(blockSize > (fileSize - bytesRead))
            blockSize = fileSize - bytesRead;

        if((u32)(loadAddress + bytesRead + blockSize) > 0x01000000)
        {
            log_printf("File ist too big\n");
            return NOT_ENOUGH_MEMORY;
        }

        int ret = recv(clientSocket, &strBuffer[0], blockSize, 0);
        if(ret <= 0)
        {
            log_printf("Failure on reading file\n");
            break;
        }

        memcpy(loadAddress + bytesRead, &strBuffer[0], ret);
        bytesRead += ret;
    }

    progressWindow.setProgress((f32)bytesRead / (f32)fileSize);

    if(bytesRead != fileSize)
    {
        log_printf("File loading not finished, %i of %i bytes received\n", bytesRead, fileSize);
        return FILE_READ_ERROR;
    }

	// Do we need to unzip this thing?
	if (haxx[4] > 0 || haxx[5] > 4)
	{
		// We need to unzip...
		if (loadAddress[0] == 'P' && loadAddress[1] == 'K' && loadAddress[2] == 0x03 && loadAddress[3] == 0x04)
		{
		    //! TODO:
		    //! mhmm this is incorrect, it has to parse the zip

            // Section is compressed, inflate
            std::string inflatedData;
            inflatedData.resize(fileSizeUnc);

            int ret = 0;
            z_stream s;
            memset(&s, 0, sizeof(s));

            s.zalloc = Z_NULL;
            s.zfree = Z_NULL;
            s.opaque = Z_NULL;

            ret = inflateInit(&s);
            if (ret != Z_OK)
                return FILE_READ_ERROR;

            s.avail_in = fileSize;
            s.next_in = (Bytef *)(&loadAddress[0]);

            s.avail_out = fileSizeUnc;
            s.next_out = (Bytef *)&inflatedData[0];

            ret = inflate(&s, Z_FINISH);
            if (ret != Z_OK && ret != Z_STREAM_END)
                return FILE_READ_ERROR;

            inflateEnd(&s);

            if(fileSizeUnc > (0x01000000 - (u32)loadAddress))
                return FILE_READ_ERROR;

            memcpy(loadAddress, &inflatedData[0], fileSizeUnc);
            fileSize = fileSizeUnc;
		}
		else
        {
            // Section is compressed, inflate
            std::string inflatedData;
            inflatedData.resize(fileSizeUnc);

			uLongf f = fileSizeUnc;
			int result = uncompress((Bytef*)&inflatedData[0], &f, (Bytef*)loadAddress, fileSize);
			if(result != Z_OK)
            {
                log_printf("uncompress failed %i\n", result);
                return FILE_READ_ERROR;
            }

			fileSizeUnc = f;
            memcpy(loadAddress, &inflatedData[0], fileSizeUnc);
            fileSize = fileSizeUnc;
        }
	}
    return fileSize;
}
