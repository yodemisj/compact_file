#include "huffman.hpp"

using namespace std;

Node::Node(int value, char c, Node* left, Node* right, bool isLeaf) {
    this->value = value;
    this->c = c;
    this->left = left;
    this->right = right;
    this->isLeaf = isLeaf;
}

bool Compare::operator()(Node* a, Node* b) {
    if (a->value == b->value) {
        return a->c > b->c;
    }
    return a->value > b->value;
}

void getHuffmanCodes(Node* root, unordered_map<char, string>& huffmanCodes, string code) {
    if (root == nullptr) return;

    if (root->isLeaf) {
        huffmanCodes[root->c] = code;
        return;
    }

    getHuffmanCodes(root->left, huffmanCodes, code + "0");
    getHuffmanCodes(root->right, huffmanCodes, code + "1");
}

string getStringByHuffmanCode(string& code, Node* huffmanTree) {
    Node* current = huffmanTree;
    string result;
    int latestIdx = 0;

    for (size_t i = 0; i < code.size(); i++) {
        if (current->isLeaf) {
            result += current->c;
            latestIdx = i;
            current = huffmanTree;
        }

        current = (code[i] == '0') ? current->left : current->right;
    }

    if (current->isLeaf) {
        code.clear();
        result += current->c;
    } else {
        code.erase(code.begin(), code.begin() + latestIdx);
    }

    return result;
}

Node* huffman(unordered_map<char, int> freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> Q;

    for (const auto& pair : freqMap) {
        Q.push(new Node(pair.second, pair.first, nullptr, nullptr, true));
    }

    while (Q.size() > 1) {
        Node* left = Q.top(); Q.pop();
        Node* right = Q.top(); Q.pop();

        Node* newNode = new Node(left->value + right->value, '\0', left, right, false);
        Q.push(newNode);
    }

    return Q.top();
}
