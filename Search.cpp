#include "Search.h"
#include "HuffmanCore.h"
#include "FileOps.h"
#include <iostream>
#include <fstream>
#include <algorithm>

//输出搜索结果，打印上下文
void displayContext(const vector<long long>& results, const string& textBits, Node* root, const string& keyword) {
    if (results.empty()) {
        cout << "未找到匹配项。" << endl;
        return;
    }

    cout << "检索成功! 出现次数: " << results.size() << endl;

    // 1. 内存解码并建立位映射
    // charStartBit[k] 存储第 k 个字符在 textBits 中的起始位位置
    vector<long long> charStartBit;
    string decodedText = "";

    // 预分配内存优化
    decodedText.reserve(textBits.length() / 5);
    charStartBit.reserve(textBits.length() / 5);

    Node* curr = root;
    long long charStart = 0;

    for (long long i = 0; i < textBits.length(); i++) {
        if (curr == root) charStart = i; // 记录字符起始位

        if (textBits[i] == '0') curr = curr->left;
        else curr = curr->right;

        if (!curr->left && !curr->right) {
            decodedText += curr->ch;
            charStartBit.push_back(charStart);
            curr = root;
        }
    }

    // 2. 打印上下文
    cout << "\n[匹配详情 (前10条)]:" << endl;
    int count = 0;
    int kwLen = keyword.length();

    for (long long targetBitPos : results) {
        if (count >= 10) { cout << "... (更多省略)" << endl; break; }

        // 二分查找字符索引
        auto it = lower_bound(charStartBit.begin(), charStartBit.end(), targetBitPos);

        if (it != charStartBit.end() && *it == targetBitPos) {
            int charIdx = distance(charStartBit.begin(), it);

            // 截取前后文
            int startCtx = max(0, charIdx - 10);
            int lenCtx = 10 + kwLen + 10;

            if (startCtx + lenCtx > decodedText.length()) {
                lenCtx = decodedText.length() - startCtx;
            }

            string context = decodedText.substr(startCtx, lenCtx);
            // 替换换行符
            replace(context.begin(), context.end(), '\n', ' ');
            replace(context.begin(), context.end(), '\r', ' ');

            cout << "位偏移 " << targetBitPos << " | 上下文: \"..." << context << "...\"" << endl;
            count++;
        }
    }
    cout << endl;
}

// 构建坏字符表
void buildBadCharTable(const string& pattern, vector<int>& badChar) {
    int m = pattern.length();
    for (int i = 0; i < 256; i++) badChar[i] = -1;
    for (int i = 0; i < m; i++) badChar[(int)pattern[i]] = i;
}

// BM搜索
vector<long long> BMBinarySearch(const string& text, const string& pattern) {
    vector<long long> positions;
    int m = pattern.length();
    int n = text.length();
    if (m > n) return positions;

    vector<int> badChar(256);
    buildBadCharTable(pattern, badChar);

    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) j--;

        if (j < 0) {
            positions.push_back(s);
            s += (s + m < n) ? m - badChar[text[s + m]] : 1;
        }
        else {
            s += max(1, j - badChar[text[s + j]]);
        }
    }
    return positions;
}

// 加载文件内容为01字符串
string loadBits(const string& filename, int startPos, int pBits) {
    ifstream in(filename, ios::binary);
    in.seekg(0, ios::end);
    long long fileSize = in.tellg();
    string bitString;
    bitString.reserve((fileSize - startPos) * 8);

    in.seekg(startPos, ios::beg);
    unsigned char byte;
    long long endPos = fileSize;

    while (in.read((char*)&byte, 1)) {
        int bitsToRead = 8;
        if (in.tellg() == endPos) bitsToRead -= pBits;
        for (int i = 0; i < bitsToRead; i++) {
            bitString += ((byte >> (7 - i)) & 1) ? '1' : '0';
        }
    }
    return bitString;
}

// void searchKeyword(const string& compressedFile, const string& keyword) {
//     // 获取文件头信息重建哈夫曼编码表
//     int dataStart, pBits;
//     map<unsigned char, long long> freqMap = readHeaderFreqMap(compressedFile, dataStart, pBits);

//     if (freqMap.empty()) { cout << "读取文件头失败" << endl; return; }

//     Node* root = buildHuffmanTree(freqMap);
//     map<unsigned char, string> codeMap;
//     generateCodes(root, "", codeMap);

//     // 将关键字转换为二进制模式串
//     string pattern = "";
//     for (unsigned char c : keyword) {
//         if (codeMap.find(c) == codeMap.end()) {
//             cout << "检索失败：关键字包含未出现的字符。" << endl;
//             freeTree(root);
//             return;
//         }
//         pattern += codeMap[c];
//     }
//     cout << "关键字编码: " << pattern << endl;

//     // 加载压缩数据
//     string textBits = loadBits(compressedFile, dataStart, pBits);

//     // 执行BM搜索
//     vector<long long> results = BMBinarySearch(textBits, pattern);

//     if (results.empty()) {
//         cout << "未找到匹配项。" << endl;
//     }
//     else {
//         cout << "检索成功! 出现次数: " << results.size() << endl;
//         cout << "位置: ";
//         for (int i = 0; i < min((int)results.size(), 10); i++) cout << results[i] << " ";
//         if (results.size() > 10) cout << "...";
//         cout << endl;
//     }

//     freeTree(root);
// }

void searchKeyword(const string& compressedFile, const string& keyword) {
    // 获取文件头信息重建哈夫曼编码表
    int dataStart, pBits;
    map<unsigned char, long long> freqMap = readHeaderFreqMap(compressedFile, dataStart, pBits);

    if (freqMap.empty()) { cout << "读取文件头失败" << endl; return; }

    Node* root = buildHuffmanTree(freqMap);
    map<unsigned char, string> codeMap;
    generateCodes(root, "", codeMap);

    // 将关键字转换为二进制模式串
    string pattern = "";
    bool unknown = false;
    for (unsigned char c : keyword) {
        if (codeMap.find(c) == codeMap.end()) {
            unknown = true; break;
        }
        pattern += codeMap[c];
    }

    if (unknown) {
        cout << "检索失败：关键字包含未出现的字符。" << endl;
        freeTree(root);
        return;
    }
    cout << "关键字编码: " << pattern << endl;

    // 加载压缩数据
    string textBits = loadBits(compressedFile, dataStart, pBits);

    // 执行 BM 搜索
    vector<long long> results = BMBinarySearch(textBits, pattern);

    //输出
    displayContext(results, textBits, root, keyword);

    freeTree(root);
}

vector<int> computeLPS(const string& pattern) {
    int m = pattern.length();
    vector<int> lps(m);
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
    return lps;
}

vector<long long> KMPSearch(const string& text, const string& pattern) {
    vector<long long> positions;
    int n = text.length();
    int m = pattern.length();
    if (m == 0) return positions;

    vector<int> lps = computeLPS(pattern);
    int i = 0; // 文本指针
    int j = 0; // 模式指针
    while (i < n) {
        if (pattern[j] == text[i]) {
            j++; i++;
        }
        if (j == m) {
            positions.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    return positions;
}

// KMP 搜索入口
void searchKeywordKMP(const string& compressedFile, const string& keyword) {
    // 重建树和编码表
    int dataStart, pBits;
    map<unsigned char, long long> freqMap = readHeaderFreqMap(compressedFile, dataStart, pBits);
    if (freqMap.empty()) { cout << "读取头失败" << endl; return; }

    Node* root = buildHuffmanTree(freqMap);
    map<unsigned char, string> codeMap;
    generateCodes(root, "", codeMap);

    // 转换关键字
    string pattern = "";
    for (unsigned char c : keyword) {
        if (codeMap.find(c) == codeMap.end()) {
            cout << "关键字包含未出现字符。" << endl; freeTree(root); return;
        }
        pattern += codeMap[c];
    }
    cout << "关键字编码: " << pattern << endl;

    // 加载数据
    string textBits = loadBits(compressedFile, dataStart, pBits);

    // 执行 KMP
    vector<long long> results = KMPSearch(textBits, pattern);

    // 显示上下文
    displayContext(results, textBits, root, keyword);

    freeTree(root);
}

bool isMatchPrefix(const string& text, const string& pattern, int pos) {
    int n = text.length();
    int m = pattern.length();
    int i = pos; // 文本指针
    int j = 0;   // 模式指针
    int starIdx = -1; // 记录上一个 '*' 的位置
    int matchIdx = -1; // 记录 '*' 匹配到的文本位置

    while (j < m) {
        // 如果 i 还没越界，且当前字符匹配 (具体字符 或 ?)
        if (i < n && (pattern[j] == '?' || pattern[j] == text[i])) {
            i++;
            j++;
        }
        // 如果遇到 '*'
        else if (pattern[j] == '*') {
            starIdx = j;      // 记录星号位置
            matchIdx = i;     // 记录当前文本位置，试图让 * 匹配 0 个字符
            j++;              // 模式串向后移
        }
        // 如果不匹配，但是之前有 '*' (回溯)
        else if (starIdx != -1) {
            j = starIdx + 1;  // 模式串重置到星号后面
            matchIdx++;       // 让星号多吞噬一个文本字符
            i = matchIdx;     // 文本指针回退到新吞噬位置的下一个
        }
        // 不匹配且无 '*'，或者文本已结束但模式没结束
        else {
            return false;
        }
    }

    // 如果循环结束，说明 pattern 走完了，匹配成功
    return true;
}

// 通配符搜索入口
void searchWildcard(const string& compressedFile, const string& wildcardPattern) {
    // 读取头信息
    int dataStart, pBits;
    map<unsigned char, long long> freqMap = readHeaderFreqMap(compressedFile, dataStart, pBits);
    if (freqMap.empty()) { cout << "读取头信息失败" << endl; return; }

    // 重建哈夫曼树
    Node* root = buildHuffmanTree(freqMap);

    // 读取二进制位流
    string bits = loadBits(compressedFile, dataStart, pBits);

    // 存解压
    // 必须还原成字符流才能处理 ? 和 *，因为它们是针对字符的，不是针对位的
    string decodedText = "";
    // 预分配内存，防止频繁扩容（估算：平均码长约5位，总大小约为 bits/5）
    decodedText.reserve(bits.length() / 5);

    Node* curr = root;
    for (char b : bits) {
        if (b == '0') curr = curr->left;
        else curr = curr->right;

        if (!curr->left && !curr->right) {
            decodedText += curr->ch;
            curr = root;
        }
    }

    cout << "模式串: " << wildcardPattern << endl;

    // 在还原的文本上进行滑动匹配
    int count = 0;
    int n = decodedText.length();
    int print = 0;
    // 这里的逻辑是：尝试每一个起始位置 i
    for (int i = 0; i < n; i++) {
        // 检查从 i 开始是否匹配
        if (isMatchPrefix(decodedText, wildcardPattern, i)) {
            print++;
            if (print == 10) cout << "..此后省略" << endl ;
            // 找到匹配
            else if (print <= 10) {
                cout << "字符偏移位置: " << i;

                // 打印匹配到的上下文片段
                cout << " (片段: \"";
                int len = 0;
                // 简单估算打印长度，打印匹配开头的一小段
                for (int k = 0; k < 15 && i + k < n; k++) {
                    // 如果遇到换行符，替换成空格以免破坏输出格式
                    char c = decodedText[i + k];
                    if (c == '\n' || c == '\r') c = ' ';
                    cout << c;
                }
                cout << "...\")" << endl;
            }
            count++;
            // 为了避免像 A******A 这种模式产生过多重叠匹配，
            // 可以在找到匹配后跳过几个字符？
            // 题目通常要求找出“所有位置”，所以这里不跳过，继续 i++
        }
    }

    if (count == 0) {
        cout << "未找到匹配项。" << endl;
    } else {
        cout << "检索结束，共找到 " << count << " 处匹配。" << endl;
    }

    freeTree(root);
}