#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <string>
#include <map>
#include "Shared.h"

using namespace std;

// 压缩文件
void compressFile(const string& inputFile, const string& outputFile);

// 解压文件
void decompressFile(const string& inputFile, const string& outputFile);

// 仅读取压缩文件头，返回频率表
map<unsigned char, long long> readHeaderFreqMap(const string& compressedFile, int& dataStartPos, int& paddingBits);


#endif