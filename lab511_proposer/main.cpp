#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[100];
Proposer proposer[100];

long long times[100], total, total1, total2, total3;
mutex total_mutex;

int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    for(int i = 0; i < 100; i++)
    {
        printf("%d\n", i);
        times[i] = gettime();
        int x = i;
        thread thd(proposer[i].init, &proposer[i], Value(x), i);
        thd.detach();
        Sleep(1000);
//        proposer[i].init(Value(x), i);
    }
    while(1);
}