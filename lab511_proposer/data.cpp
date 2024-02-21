//
// Created by Steven's WIn10 on 2023/3/30.
//
#include <cstdio>

#include "data.h"

long long gettime() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    return value.count();
}

Value::Value(int val) : val(val) {}

Value::Value() {}

void Value::print() {
    printf("--------------------VALUE-------------------\n");
    printf("val = %d\n", this->val);
    printf("---------------------------------------------------\n");
}

PrepareData::PrepareData(pair<long long, int> N, int id) : N(N), id() {}

PrepareData::PrepareData() {}

void PrepareData::print() {
    printf("--------------------PREPAREDATA-------------------\n");
    printf("N.first = %lld\n", this->N.first);
    printf("N.second = %lld\n", this->N.second);
    printf("id = %d\n", this->id);
    printf("---------------------------------------------------\n");
}

PromiseData::PromiseData(bool state, pair<long long, int> MaxN, Value d, int ip, int id) : state(state), MaxN(MaxN), d(d),
                                                                                   ip(ip), id(id) {}

PromiseData::PromiseData() {}

void PromiseData::print() {
    printf("-------------------PROMISEDATA-------------------\n");
    printf("val = %d\n", this->d.val);
    printf("state = %d\n", this->state);
    printf("MaxN.first = %lld\n", this->MaxN.first);
    printf("MaxN.second = %d\n", this->MaxN.second);
    printf("ip = %d\n", this->ip);
    printf("id = %d\n", this->id);
    printf("---------------------------------------------------\n");
}

AcceptData::AcceptData(pair<long long, int> N, Value d, int id) : N(N), d(d), id(id) {}

AcceptData::AcceptData() {}

void AcceptData::print() {
    printf("-------------------ACCEPTDATA-------------------\n");
    printf("N.first = %lld\n", this->N.first);
    printf("N.second = %d\n", this->N.second);
    printf("val = %d\n", this->d.val);
    printf("id = %d\n", this->id);
    printf("---------------------------------------------------\n");
}

AcceptedData::AcceptedData(bool state, int ip, int id) : state(state), ip(ip), id(id) {}

AcceptedData::AcceptedData() {}

void AcceptedData::print() {
    printf("--------------------ACCEPTEDDATA-------------------\n");
    printf("state = %d\n", this->state);
    printf("ip = %d\n", this->ip);
    printf("id = %d\n", this->id);
    printf("---------------------------------------------------\n");
}

CommitData::CommitData(bool state, Value d, int id) : state(state), d(d), id(id) {}

CommitData::CommitData() {}

void CommitData::print() {
    printf("--------------------COMMITDATA-------------------\n");
    printf("state = %d\n", this->state);
    printf("val = %d\n", this->d.val);
    printf("id = %d\n", this->id);
    printf("---------------------------------------------------\n");
}