#ifndef HUFFMAN_CORE_H
#define HUFFMAN_CORE_H

#include "Shared.h"
#include <queue>

// 构建哈夫曼树
Node* buildHuffmanTree(const map<unsigned char, long long>& freqMap);

// 递归生成编码表
void generateCodes(Node* root, string str, map<unsigned char, string>& codeMap);

// 释放树内存
void freeTree(Node* root);

#endif