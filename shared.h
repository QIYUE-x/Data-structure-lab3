// Shared.h
#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <map>
#include <vector>

using namespace std;

// 哈夫曼树节点定义
struct Node {
    unsigned char ch;
    long long freq;
    Node* left, * right;

    Node(unsigned char c, long long f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// 优先队列比较函数
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;   //小顶堆
    }
};

#endif