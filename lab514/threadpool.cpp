#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
const int MAX_CLIENT_NUM = 100;
const int PORT = 1234;
const int BUFFER_SIZE = 1024;
SOCKET client_socket[MAX_CLIENT_NUM];
int client_num = 0;
HANDLE hMutex;
DWORD WINAPI ServerThread(LPVOID lpParameter);
DWORD WINAPI ClientThread(LPVOID lpParameter);
int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
    listen(server_socket, SOMAXCONN);
    hMutex = CreateMutex(NULL, FALSE, NULL);
    while (true)
    {
        SOCKADDR_IN client_addr;
        int len = sizeof(client_addr);
        SOCKET client_sock = accept(server_socket, (SOCKADDR*)&client_addr, &len);
        WaitForSingleObject(hMutex, INFINITE);
        client_socket[client_num++] = client_sock;
        ReleaseMutex(hMutex);
        HANDLE hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)(client_num - 1), 0, NULL);
        CloseHandle(hThread);
    }
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
DWORD WINAPI ServerThread(LPVOID lpParameter)
{
    int index = *(int*)lpParameter;
    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int ret = recv(client_socket[index], buffer, sizeof(buffer), 0);
        if (ret == SOCKET_ERROR || ret == 0)
        {
            WaitForSingleObject(hMutex, INFINITE);
            for (int i = index; i < client_num - 1; i++)
            {
                client_socket[i] = client_socket[i + 1];
            }
            client_num--;
            ReleaseMutex(hMutex);
            break;
        }
        cout << "Client " << index << ": " << buffer << endl;
        for (int i = 0; i < client_num; i++)
        {
            if (i != index)
            {
                send(client_socket[i], buffer, strlen(buffer), 0);
            }
        }
    }
    closesocket(client_socket[index]);
    return 0;
}
DWORD WINAPI ClientThread(LPVOID lpParameter)
{
    int index = *(int*)lpParameter;
    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        cin.getline(buffer, sizeof(buffer));
        send(client_socket[index], buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }
    }
    return 0;
}