#include "InitSock.h"
#include <iostream>
#include <httplib.h>
using namespace std;

void GetAllIpAddress()
{
    char cHost[256] = { 0 };
    ::gethostname(cHost, 256);  //取得本地主机名
    hostent* pHost = ::gethostbyname(cHost);    //通过主机名得到地址信息

    //打印出所有Ip地址
    in_addr addr;
    for (int i = 0;; i++)
    {
        char*p = pHost->h_addr_list[i];
        if (p == NULL)
        {
            break;
        }
        memcpy(&addr.S_un.S_addr, p, pHost->h_length);
        char*cIp = ::inet_ntoa(addr);
        cout << "IP:" << cIp << endl;
    }
}

void GetIpAddress()
{

    char cHost[256] = { 0 };
    ::gethostname(cHost, 256);  //取得本地主机名
    hostent* pHost = ::gethostbyname(cHost);    //通过主机名得到地址信息

    //打印本机Ip地址
    in_addr addr;
    char*p = pHost->h_addr_list[0];
    if (p == NULL)
    {
        return;
    }
    memcpy(&addr.S_un.S_addr, p, pHost->h_length);
    char*cIp = ::inet_ntoa(addr);
    cout << "IP:" << cIp << endl;
    
}

int main()
{
    //初始化Winsock库
    CInitSock initSock;

    //打印所有Ip地址
    GetAllIpAddress();
    //打印本机Ip地址
    //GetIpAddress();
    return 0;
}