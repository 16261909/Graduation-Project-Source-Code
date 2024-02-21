#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

const int PORT = 12345;

int main()
{
    //��ʼ��WSA
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //����socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    //��socket
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("0.0.0.0");
    sockAddr.sin_port = htons(PORT);
    bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //����socket
    listen(sock, 20);

    //���ܿͻ�������
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(sock, (SOCKADDR*)&clntAddr, &nSize);

    //��ͻ��˷�������
    char buf[256] = {0};
    while(1)
    {
        memset(buf, 0, sizeof buf);
        gets(buf);
        send(clntSock, buf, strlen(buf) + sizeof(char), NULL);
        if(strcmp(buf, "end") == 0)
            break;
    }

    //�ر�socket
    closesocket(clntSock);
    closesocket(sock);

    //��ֹWSA
    WSACleanup();

    puts("!");
    while(1);
    return 0;
}