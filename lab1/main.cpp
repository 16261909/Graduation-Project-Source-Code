#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[100];
Proposer proposer[100];


int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    proposer[0].init(Value(1), 0);
    while(1);
}