#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include "huffman.hpp"
#include <fstream>
#include <string>

using namespace std;

unordered_map<char, int> calcFreq(const string& path);

void compactFile(const string& sourcePath, const string& destPath, Node* huffmanTree, unordered_map<char, int> freqMap);

void descompactFile(const string& sourcePath, const string& destPath);

#endif 
