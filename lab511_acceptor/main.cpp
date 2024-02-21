#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[100];
Proposer proposer[100];


mutex total_mutex;
long long times[100], total, total1, total2;

int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    while(1);

}