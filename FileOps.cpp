#include "FileOps.h"
#include "HuffmanCore.h"
#include <fstream>
#include <iostream>

// 压缩实现
void compressFile(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    if (!in.is_open()) { cerr << "无法打开输入文件" << endl; return; }

    map<unsigned char, long long> freqMap;
    unsigned char ch;
    long long totalBytes = 0;
    while (in.read((char*)&ch, 1)) {
        freqMap[ch]++;
        totalBytes++;
    }
    in.close();

    if (totalBytes == 0) return;

    // 建树和生成编码
    Node* root = buildHuffmanTree(freqMap);
    map<unsigned char, string> codeMap;
    generateCodes(root, "", codeMap);

    ofstream out(outputFile, ios::binary);

    // 1. 写入头信息：[MapSize] -> [Char, Freq]... -> [PaddingPlaceholder]
    int mapSize = freqMap.size();
    out.write((char*)&mapSize, sizeof(mapSize));
    for (auto pair : freqMap) {
        out.write((char*)&pair.first, sizeof(pair.first));
        out.write((char*)&pair.second, sizeof(pair.second));
    }

    // 占位填充位
    long posPadding = out.tellp();
    int paddingBits = 0;
    out.write((char*)&paddingBits, sizeof(paddingBits));

    // 2. 写入数据
    in.open(inputFile, ios::binary);
    unsigned char byte = 0;
    int bitCount = 0;

    while (in.read((char*)&ch, 1)) {
        string code = codeMap[ch];
        for (char c : code) {
            if (c == '1') byte |= (1 << (7 - bitCount));
            bitCount++;
            if (bitCount == 8) {
                out.write((char*)&byte, 1);
                byte = 0; bitCount = 0;
            }
        }
    }
    // 处理尾部
    if (bitCount > 0) {
        paddingBits = 8 - bitCount;
        out.write((char*)&byte, 1);
    }

    // 回写填充位
    out.seekp(posPadding);
    out.write((char*)&paddingBits, sizeof(paddingBits));

    in.close();
    out.close();
    freeTree(root);

    ifstream checkSize(outputFile, ios::binary | ios::ate); // 打开时直接定位到文件末尾
    long long compressedSize = checkSize.tellg();           // 获取当前位置（即文件大小）
    checkSize.close();

    cout << "压缩完成!" << endl;
    cout << "原始文件大小: " << totalBytes << " Bytes" << endl;
    cout << "压缩文件大小: " << compressedSize << " Bytes" << endl;

    double ratio = (double)compressedSize / totalBytes * 100.0;
    cout.precision(2); // 设置精度
    cout << fixed << "压缩率: " << ratio << "%" << endl;
}

// 解压实现
void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    if (!in.is_open()) { cerr << "无法打开压缩文件" << endl; return; }

    // 读取头部
    int mapSize;
    in.read((char*)&mapSize, sizeof(mapSize));
    map<unsigned char, long long> freqMap;
    for (int i = 0; i < mapSize; i++) {
        unsigned char c; long long f;
        in.read((char*)&c, sizeof(c));
        in.read((char*)&f, sizeof(f));
        freqMap[c] = f;
    }
    int pBits;
    in.read((char*)&pBits, sizeof(pBits));

    // 重建树
    Node* root = buildHuffmanTree(freqMap);
    Node* curr = root;

    ofstream out(outputFile, ios::binary);
    unsigned char byte;

    // 确定文件结束位置
    long long dataStart = in.tellg();
    in.seekg(0, ios::end);
    long long endPos = in.tellg();
    in.seekg(dataStart, ios::beg);

    while (in.read((char*)&byte, 1)) {
        int bitsToRead = 8;
        if (in.tellg() == endPos) bitsToRead -= pBits;

        for (int i = 0; i < bitsToRead; i++) {
            int bit = (byte >> (7 - i)) & 1;
            if (bit == 0) curr = curr->left;
            else curr = curr->right;

            if (!curr->left && !curr->right) {
                out.write((char*)&curr->ch, 1);
                curr = root;
            }
        }
    }

    in.close();
    out.close();
    freeTree(root);
    cout << "解压完成!" << endl;
}

// 辅助函数：读取频率表
map<unsigned char, long long> readHeaderFreqMap(const string& compressedFile, int& dataStartPos, int& paddingBits) {
    ifstream in(compressedFile, ios::binary);
    map<unsigned char, long long> freqMap;
    if (!in.is_open()) return freqMap;

    int mapSize;
    in.read((char*)&mapSize, sizeof(mapSize));
    for (int i = 0; i < mapSize; i++) {
        unsigned char c; long long f;
        in.read((char*)&c, sizeof(c));
        in.read((char*)&f, sizeof(f));
        freqMap[c] = f;
    }
    in.read((char*)&paddingBits, sizeof(paddingBits));
    dataStartPos = in.tellg();
    in.close();
    return freqMap;
}