//
// Created by Steven's WIn10 on 2023/3/29.
//

#ifndef LAB1_NETWORK_H
#define LAB1_NETWORK_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <set>
#include <vector>

#include <WinSock2.h>

#include "paxos.h"
#include "data.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int MAXIP = 256;

extern Acceptor acceptor[20];
extern Proposer proposer[20];

inline int next();

int getlow_c(char* addr);

int getlow_s(string addr);

string getwhole(int ip);

void str2char(char *ret, string str);

void _init();

void end();

void client_search(int i);

void client_send(int i, char* msg);

void server_recv(int id);

void server();

void for_client_search();

void for_client_send(char* msg);

void for_for_client_search();

void serverinit();

void clientinit();

#endif //LAB1_NETWORK_H

