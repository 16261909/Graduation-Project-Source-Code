#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[100];
Proposer proposer[100];

long long times;

int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    getchar();
    times = gettime();
    proposer[0].init(Value(2), 0);
    while(1);
}