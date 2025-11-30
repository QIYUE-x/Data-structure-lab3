#include "HuffmanCore.h"

Node* buildHuffmanTree(const map<unsigned char, long long>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    if (pq.empty()) return nullptr;

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* sum = new Node('\0', left->freq + right->freq);
        sum->left = left;
        sum->right = right;
        pq.push(sum);
    }
    return pq.top();
}

void generateCodes(Node* root, string str, map<unsigned char, string>& codeMap) {
    if (!root) return;
    if (!root->left && !root->right) {
        codeMap[root->ch] = str;
    }
    generateCodes(root->left, str + "0", codeMap);
    generateCodes(root->right, str + "1", codeMap);
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}