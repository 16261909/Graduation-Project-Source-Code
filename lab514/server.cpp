#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

const int PORT = 12345;

int main()
{
    //初始化WSA
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    //绑定socket
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("0.0.0.0");
    sockAddr.sin_port = htons(PORT);
    bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //监听socket
    listen(sock, 20);

    //接受客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(sock, (SOCKADDR*)&clntAddr, &nSize);

    //向客户端发送数据
    char buf[256] = {0};
    while(1)
    {
        memset(buf, 0, sizeof buf);
        gets(buf);
        send(clntSock, buf, strlen(buf) + sizeof(char), NULL);
        if(strcmp(buf, "end") == 0)
            break;
    }

    //关闭socket
    closesocket(clntSock);
    closesocket(sock);

    //终止WSA
    WSACleanup();

    puts("!");
    while(1);
    return 0;
}