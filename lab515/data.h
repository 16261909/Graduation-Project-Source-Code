//
// Created by Steven's WIn10 on 2023/3/29.
//

#ifndef LAB1_DATA_H
#define LAB1_DATA_H

#include <time.h>
#include <utility>
#include <chrono>

using namespace std;

long long gettime();

struct Value {
    int val;

    Value();
    Value(int val);
    void print();
};

struct PrepareData {
    pair<long long, int> N;
    int id;

    PrepareData();
    PrepareData(pair<long long, int> N, int id);
    void print();
};

struct PromiseData {
    bool state;
    pair<long long, int> MaxN;
    struct Value d;
    int ip;
    int id;

    PromiseData();
    PromiseData(bool state, pair<long long, int> MaxN, Value d, int ip, int id);
    void print();
};

struct AcceptData {
    pair<long long, int> N;
    struct Value d;
    int id;

    AcceptData();
    AcceptData(pair<long long, int> N, Value d, int id);
    void print();
};

struct AcceptedData {
    bool state;
    int ip;
    int id;

    AcceptedData();
    AcceptedData(bool state, int ip, int id);
    void print();
};

struct CommitData {
    bool state;
    Value d;
    int id;

    CommitData();
    CommitData(bool state, Value d, int id);
    void print();
};

#endif //LAB1_DATA_H
