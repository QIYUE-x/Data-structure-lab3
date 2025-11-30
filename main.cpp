// 必须先定义这个宏，防止 Windows 的 min/max 宏与 C++ 标准库冲突
#define NOMINMAX
// 必须把 windows.h 放在最前面，防止 byte 类型冲突
#include <windows.h>

#include <iostream>
#include <limits>
#include "FileOps.h"
#include "Search.h"

using namespace std;

// 去除路径两端可能存在的引号（Windows复制路径时常带引号）
string cleanPath(string path) {
    if (path.empty()) return path;
    if (path.front() == '"') path.erase(0, 1);
    if (path.back() == '"') path.pop_back();
    return path;
}

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    string inputFile, compressedFile, decompressedFile, keyword;
    int choice;

    while (true) {
        cout << "\n=== 哈夫曼压缩与检索系统 ===" << endl;
        cout << "1. 压缩文件" << endl;
        cout << "2. 解压文件" << endl;
        cout << "3. BM算法检索" << endl;
        cout << "4. KMP算法检索" << endl;
        cout << "5. 通配符检索 (支持*和?)" << endl;
        cout << "6. 退出" << endl;
        cout << "请选择: ";
        // 读取选项，如果输入非数字处理错误
        if (!(cin >> choice)) {
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清空缓冲区
            cout << "无效输入，请输入数字。" << endl;
            continue;
        }

        // 清除缓冲区中留下的换行符，否则 getline 会直接读到一个空行
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            cout << "输入源文件路径: ";
            getline(cin, inputFile); // 使用 getline 读取整行，支持空格
            inputFile = cleanPath(inputFile);

            cout << "输入输出压缩文件路径: ";
            getline(cin, compressedFile);
            compressedFile = cleanPath(compressedFile);

            compressFile(inputFile, compressedFile);
            break;

        case 2:
            cout << "输入压缩文件路径: ";
            getline(cin, compressedFile);
            compressedFile = cleanPath(compressedFile);

            cout << "输入解压后文件名: ";
            getline(cin, decompressedFile);
            decompressedFile = cleanPath(decompressedFile);

            decompressFile(compressedFile, decompressedFile);
            break;

        case 3:
            cout << "输入压缩文件路径: ";
            getline(cin, compressedFile);
            compressedFile = cleanPath(compressedFile);

            cout << "输入要检索的单词: ";
            cin >> keyword; // 单词通常不带空格，可以用 cin
            //如果上面用了cin，下一次循环前的 ignore 会处理掉换行符，逻辑是通的

            searchKeyword(compressedFile, keyword);
            break;

        case 4:
            cout << "输入压缩文件: "; getline(cin, compressedFile); compressedFile = cleanPath(compressedFile);
            cout << "输入关键字: "; cin >> keyword;
            searchKeywordKMP(compressedFile, keyword);
            break;

        case 5:
            cout << "输入压缩文件: "; getline(cin, compressedFile); compressedFile = cleanPath(compressedFile);
            cout << "输入通配符模式 (如 AA??BB): "; cin >> keyword;
            searchWildcard(compressedFile, keyword);
            break;

        case 6: return 0;

        default:
            cout << "无效选项，请重试。" << endl;
        }
    }
    return 0;
}