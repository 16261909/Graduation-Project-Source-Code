//
// Created by Steven's WIn10 on 2023/3/29.
//
#include "network.h"

using namespace std::chrono_literals;

const int PORT = 10003;
const int MAX_THREAD_NUM = 11;

vector<string> iplist = {"192.168.31.6", "192.168.31.62", "192.168.31.117"};
bool isconn[MAXIP];
SOCKET client_sock[MAXIP], server_sock[MAX_THREAD_NUM + 1];
int server_sock_id, localhost;

inline int next() {
    return (server_sock_id++) % MAX_THREAD_NUM + 1;
}

int getlow_c(char *addr) {
    int x = 0, ret = 0;
    for (int i = strlen(addr) - 1; i >= 0; i--) {
        if (addr[i] == '.')break;
        x = x * 10 + addr[i] - '0';
    }
    while (x) {
        ret = ret * 10 + x % 10;
        x /= 10;
    }
    return ret;
}

int getlow_s(string addr) {
    int x = 0, ret = 0;
    for (int i = addr.length() - 1; i >= 0; i--) {
        if (addr[i] == '.')break;
        x = x * 10 + addr[i] - '0';
    }
    while (x) {
        ret = ret * 10 + x % 10;
        x /= 10;
    }
    return ret;
}

string getwhole(int ip) {
    int rip = 0;
    while (ip) {
        rip = rip * 10 + ip % 10;
        ip /= 10;
    }
    string ret = "";
    for (int i = 0, cnt = 0, len = iplist[0].length(); i < len; i++) {
        ret += iplist[0][i];
        if (iplist[0][i] == '.') {
            if (++cnt == 3)break;
        }
    }
    while (rip) {
        ret += rip % 10 + '0';
        rip /= 10;
    }
    return ret;
}

void str2char(char *ret, string str) {
    for (int i = 0, len = str.length(); i < len; i++) {
        ret[i] = str[i];
    }
}

void _init() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return;
    }
    server_sock[0] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock[0] == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        return;
    }
}

void end() {
    WSACleanup();
}

void client_search(int i) {
    printf("Trying to connect: %d\n", i);
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    char addr[30] = {0};
    str2char(addr, getwhole(i));
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_port = htons(PORT);
    int iResult = connect(client_sock[i], (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (iResult == SOCKET_ERROR) {
        cout << WSAGetLastError() << endl;
    }
    else
    {
        cout << "Successfully connected to " << i << endl;
        isconn[i] = 1;
    }
}

void client_send(int i, char *msg) {
//    printf("sendtime=%lld buffer[0]=%d\n", gettime(), msg[0]);
    int iResult;
    if (msg[0] == 0) {
        iResult = send(client_sock[i], msg, sizeof(struct PrepareData) + 1, 0);
    } else if (msg[0] == 1) {
        iResult = send(client_sock[i], msg, sizeof(struct PromiseData) + 1, 0);
    } else if (msg[0] == 2) {
        iResult = send(client_sock[i], msg, sizeof(struct AcceptData) + 1, 0);
    } else if (msg[0] == 3) {
        iResult = send(client_sock[i], msg, sizeof(struct AcceptedData) + 1, 0);
    } else if (msg[0] == 4) {
        iResult = send(client_sock[i], msg, sizeof(struct CommitData) + 1, 0);
    }
    if (iResult == SOCKET_ERROR) {
        std::cerr << "sending to id = " << i << " failed: " << WSAGetLastError() << std::endl;
        isconn[i] = 0;
        return;
    }
}

void server_recv(int id) {

    printf("recvid:%d\n", id);
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof buffer);
//        printf("prerecvtime=%lld\n", gettime());
        int iResult = recv(server_sock[id], buffer, sizeof(buffer), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
//        printf("recvtime=%lld buffer[0]=%d\n", gettime(), buffer[0]);
        if (buffer[0] == 0) {
            PrepareData pre;
            memcpy(&pre, buffer + 1, sizeof(PrepareData));
            pre.print();
            thread thd(acceptor[pre.id].receivePrepare, &acceptor[pre.id], pre);
            thd.detach();
        } else if (buffer[0] == 1) {
            PromiseData pro;
            memcpy(&pro, buffer + 1, sizeof(PromiseData));
            pro.print();
            thread thd(proposer[pro.id].receivePromise, &proposer[pro.id], pro);
            thd.detach();
        } else if (buffer[0] == 2) {
            AcceptData acc;
            memcpy(&acc, buffer + 1, sizeof(AcceptData));
//            acc.print();
            thread thd(acceptor[acc.id].receiveAccept, &acceptor[acc.id], acc);
            thd.detach();
        } else if (buffer[0] == 3) {
            AcceptedData acc;
            memcpy(&acc, buffer + 1, sizeof(AcceptedData));
            acc.print();
            thread thd{proposer[acc.id].receiveAccepted, &proposer[acc.id], acc};
            thd.detach();
        } else if (buffer[0] == 4) {
            CommitData com;
            memcpy(&com, buffer + 1, sizeof(CommitData));
//            com.print();
            thread thd(acceptor[com.id].receiveCommit, &acceptor[com.id], com);
            thd.detach();
        }
//        std::cout << getwhole(id) << ": " << buffer << std::endl;
    }
}


void server_init() {
    char ip[30] = {0};
    struct hostent *phostinfo = gethostbyname("");
    for (int i = 0; phostinfo != NULL && phostinfo->h_addr_list[i] != NULL; i++) {
        char *pszAddr = inet_ntoa(*(struct in_addr *) phostinfo->h_addr_list[i]);
        if (pszAddr[0] == '1' && pszAddr[1] == '9' && pszAddr[2] == '2' && pszAddr[4] == '1' && pszAddr[5] == '6' &&
            pszAddr[6] == '8')
            strcpy(ip, pszAddr);
    }
    int lip = getlow_c(ip);
    localhost = lip;
    for(int i = 0; i < 100; i++)acceptor[i].init(i);
    struct sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);

    int iResult = bind(server_sock[0], (struct sockaddr *) &local_addr, sizeof(local_addr));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_sock[0]);
        WSACleanup();
        return;
    }
    iResult = listen(server_sock[0], MAX_THREAD_NUM);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        return;
    }
    printf("localhost: %s\n", ip);
    while (1) {
        struct sockaddr_in client_addr{};
        int client_addr_len = sizeof(client_addr);
        SOCKET sock = accept(server_sock[0], (struct sockaddr *) &client_addr, &client_addr_len);
        if (sock == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
        }

        int id = next();
        server_sock[id] = sock;
        thread sr(server_recv, id);
        sr.detach();
    }
}


void for_client_search() {
    vector<thread> threads;
    for (auto ipstr: iplist) {
        int ip = getlow_s(ipstr);
        if (isconn[ip] == 1)continue;
        client_sock[ip] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_sock[ip] == INVALID_SOCKET) {
            std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
            return;
        }
        threads.push_back(thread(client_search, ip));
    }
    for (auto &&t: threads) {
        t.join();
    }
}

void for_client_send(char *msg) {
    vector<thread> threads;
    for (auto ipstr: iplist) {
        int ip = getlow_s(ipstr);
        if (isconn[ip] == 0)continue;
        threads.push_back(thread(client_send, ip, msg));
    }
    for (auto &&t: threads) {
        t.join();
    }
}

void for_for_client_search() {
    while (1) {
        this_thread::sleep_for(5000ms);
        thread fcsearch(for_client_search);
        fcsearch.join();
    }
}

void serverinit() {
    _init();
    thread s(server_init);
    s.detach();
}

void clientinit()
{
    thread fcsearch(for_client_search);
    fcsearch.join();
    cout << "Finish Initializing. Ip Online:" << endl;
    //    for(auto ipstr : iplist)
    //    {
    //        if(isconn[getlow_s(ipstr)] || getlow_s(ipstr) == localhost)
    //        {
    //            cout << ipstr << endl;
    //        }
    //    }
    thread ffcsearch(for_for_client_search);
    ffcsearch.detach();
}