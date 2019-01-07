#ifndef MINHASH_HPP
#define MINHASH_HPP

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include "clhash.h"

using namespace std;

class minhasher{
    private:
        int hash_num = 256;  // bxr = 16x16
        int shingle_num = 9;
        vector<void*> randoms;
    public:
        minhasher();
        ~minhasher();
        vector<uint64_t> signature(ifstream *inFile);
        float compute_similarity(ifstream *f1, ifstream *f2);
        // bool load(const char *path);
        // bool save(const char *path);
};


#endif