#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>

using namespace std;

// 关键字检索入口
void searchKeyword(const string& compressedFile, const string& keyword);

// 使用 KMP 算法进行精确搜索
void searchKeywordKMP(const string& compressedFile, const string& keyword);

//支持通配符的搜索 (?, *)
void searchWildcard(const string& compressedFile, const string& wildcardPattern);

#endif