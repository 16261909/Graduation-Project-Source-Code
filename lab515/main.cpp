#include "network.h"
#include "paxos.h"
#include "data.h"

using namespace std;

Acceptor acceptor[100];
Proposer proposer[100];

int b[4] = {0, 1, 2 ,3};
int c[4] = {0, 1, 3, 6};
int cnt1, cnt2, cnt3;

int main()
{
    puts("Server is starting.");
    serverinit();
    getchar();
    puts("Client is Connecting.");
    clientinit();
    getchar();
    for(int i = 0; i < 100; i++)
    {
        puts("#");
        thread thd(proposer[i].init, &proposer[i], Value(1), i);
        thd.detach();
        puts("!");
        Sleep(4000);
        puts("@");
    }
    while(1);
}