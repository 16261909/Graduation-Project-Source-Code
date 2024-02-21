#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[20];
Proposer proposer[20];

int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    for(int i = 0; i < 20; i++)
    {
        int x;
        cin >> x;
        proposer[i].init(Value(x), i);
    }
}