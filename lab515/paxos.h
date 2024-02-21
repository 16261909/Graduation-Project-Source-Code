#ifndef LAB1_PAXOS_H
#define LAB1_PAXOS_H

#include <utility>
#include <mutex>
#include <atomic>
#include <shared_mutex>
#include <cassert>
#include <thread>

#include <windows.h>

#include "data.h"
#include "network.h"

using namespace std;

extern int b[4];
extern int c[4];

extern int cnt1, cnt2, cnt3;

const int T2 = 3000;//ms

extern int localhost;
extern vector<string> iplist;

extern long long times;

extern int getlow_c(char *addr);

extern int getlow_s(string addr);

extern void client_send(int ip, char *msg);

extern void for_client_send(char *msg);



class Proposer {
public:
    Proposer() {}

    void init(const Value &value, int ID) {
        cnt++;
        acc.id = pre.id = com.id = ID;
        data = value;

//        acc.N = pre.N = make_pair(gettime()%10000, localhost);
        long long x = gettime()/20000, old = 1, y = localhost;

        long long idl = c[old-1] + 1, idr = c[old];
        idl = (idl + x) % c[3] + 1;
        idr = (idr + x) % c[3] + 1;

        printf("t=%lld,%lld l=%lld r=%lld ", x % c[3], x%30, idl, idr);

        x = (idl <= idr) ? idr : c[3];
//        x = old;
        printf("id=%lld\n", x);


        acc.N = pre.N = make_pair(x, y);
        MaxN = -1;
        proCnt.store(0);
        accCnt.store(0);
        state.store(0);//init
        memset(buffer, 0, sizeof buffer);
        memset(ispro, 0, sizeof ispro);
        sendPrepare();
    }

    void sendPrepare() {
        puts("1");
        if (state.load() != 0)return;
        state.store(1);//prepare
        buffer_mutex.lock();
//        memset(buffer, 0, sizeof buffer);
        buffer[0] = 0;
        Memory_mutex.lock();
        memcpy(buffer + 1, &pre, sizeof(struct PrepareData));
        Memory_mutex.unlock();
        buffer[sizeof(struct PrepareData) + 1] = 0;
        state.store(2);//promise
        for_client_send(buffer);

        buffer_mutex.unlock();

        Sleep(T2);
//        printf("%d %d\n", state.load(), proCnt.load());
        if (state.load() == 2) {
            cout << "Not enough promise. Received promise(s) from " << proCnt.load() << " node(s)." << endl;
        }
    }

    void receivePromise(const PromiseData &d) {
        if (state.load() != 2 || !d.state)return;
        printf("%d\n", d.ip);
        ispro[d.ip] = 1;
        Memory_mutex.lock();
        if (d.MaxN > make_pair(pre.N.first, pre.N.second) || MaxN == -1 && d.MaxN.first != -1) {
            MaxN = d.MaxN.first;
            data = d.d;
        }
        Memory_mutex.unlock();
        if (proCnt.fetch_add(1) == iplist.size() / 2) {
            sendAccept();
        }
    }

    void sendAccept() {
        assert(state.load() == 2);
        Sleep(2000);
        state.store(3);//accept
        Memory_mutex.lock();
        acc.d = data;
        buffer_mutex.lock();
        buffer[0] = 2;
        memcpy(buffer + 1, &acc, sizeof(struct AcceptData));
        Memory_mutex.unlock();
        buffer[sizeof(struct AcceptData) + 1] = 0;
        state.store(4);//accepted
        vector<thread> threads;
//        printf("ispro[62]=%d %d\n",ispro[62], proCnt.load());
        for (const auto &ipstr: iplist) {
            int ip = getlow_s(ipstr);
            if(!ispro[ip]) continue;
//            printf("%d\n",ip);
            threads.push_back(thread(client_send, ip, buffer));
        }
        for (auto &&t: threads) {
            t.join();
        }
        buffer_mutex.unlock();

        Sleep(T2);
//        printf("!%d %d\n", state.load(), accCnt.load());
        if (state.load() == 4) {
            cout << "Not enough accepted. Received promise(s) from " << accCnt.load() << " node(s)." << endl;
        }
    }

    void receiveAccepted(const AcceptedData &d) {
        if (state.load() != 4 || !d.state)return;
        if (accCnt.fetch_add(1) == iplist.size() / 2) {
            sendCommit();
        }
    }

    void sendCommit() {
        assert(state.load() == 4);
        state.store(5);//commit
        Memory_mutex.lock();
        com.d = data;
        buffer_mutex.lock();
        buffer[0] = 4;
        memcpy(buffer + 1, &com, sizeof(struct CommitData));
        Memory_mutex.unlock();
        for_client_send(buffer);
        buffer_mutex.unlock();
    }

private:
    char buffer[1024];
    bool ispro[256];
    long long MaxN;
    struct Value data;
    struct PrepareData pre;
    struct AcceptData acc;
    struct CommitData com;
    atomic<int> proCnt, accCnt, state;
    mutex buffer_mutex, Memory_mutex;

    int cnt = 0;
};

class Acceptor {
public:
    Acceptor() {}

    void init(int ID) {
        pro.id = acc.id = ID;
        iscommitted.store(false);
        MaxN = make_pair(-1, -1);
        pro.state = true;
        pro.MaxN = make_pair(-1, -1);
        pro.d = Value();
        acc.ip = pro.ip = localhost;
        acc.state = true;
    }

    void receivePrepare(const PrepareData &d) {
        if (iscommitted.load())
            return;
        Acceptor_mutex.lock();
        assert(d.N != MaxN);
        if (d.N > MaxN) {
            MaxN = d.N;
            pro.state = 1;
        } else {
            pro.state = 0;
        }
        buffer_mutex.lock();
        buffer[0] = 1;
        memcpy(buffer + 1, &pro, sizeof(struct PromiseData));
        buffer[sizeof(struct PromiseData) + 1] = 0;
        Acceptor_mutex.unlock();
        client_send(d.N.second, buffer);
        buffer_mutex.unlock();
    }

    void receiveAccept(const AcceptData &d) {
        if (iscommitted.load())
            return;
        Acceptor_mutex.lock();
        assert(d.N <= MaxN);
        if (d.N < MaxN) {
            acc.state = 0;
        } else if (d.N == MaxN) {
            acc.state = 1;
            pro.d = d.d;
            pro.MaxN = d.N;;
        }
        buffer_mutex.lock();
        buffer[0] = 3;
        memcpy(buffer + 1, &acc, sizeof(struct AcceptedData));
        buffer[sizeof(struct AcceptedData) + 1] = 0;
        Acceptor_mutex.unlock();
        client_send(d.N.second, buffer);
        buffer_mutex.unlock();
    }

    void receiveCommit(const CommitData &d) {
        assert(!iscommitted.load());
        iscommitted.store(1);
        cout << "Committed value: " << d.d.val << endl;
        if(d.d.val == 1)cnt1++;
        if(d.d.val == 2)cnt2++;
        if(d.d.val == 3)cnt3++;
        printf("%d %d %d\n", cnt1, cnt2, cnt3);
    }

private:
    char buffer[1024];
    pair<long long, int> MaxN;
    PromiseData pro;
    AcceptedData acc;
    atomic<bool> iscommitted;
    mutex buffer_mutex, Acceptor_mutex;
};

#endif //LAB1_PAXOS_H
