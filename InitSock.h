#pragma once
#include <winsock2.h>
#pragma comment(lib,"ws2_32")       //链接ws2_32.lib库

class CInitSock
{
public:
    CInitSock(BYTE minorVer=2, BYTE major = 2)      //载入Winsock库
    {
        WSADATA wsaData;
        WORD sockVersion = MAKEWORD(minorVer, major);
        if (::WSAStartup(sockVersion, &wsaData) != 0)
        {
            return;
        }
    }
    ~CInitSock()
    {
        ::WSACleanup(); //释放Winsock库
    }
};