#include "buckets.hpp"
#include <iostream>
#include <fstream>

buckets::buckets(uint64_t size){
    this->size = size;
    this->bucket = vector<vector<string> >(this->size, vector<string>());
    this->random = get_random_key_for_clhash(UINT64_C(0x23a23cf5023c3c81),UINT64_C(0xb3816f6a2c68e523));
}

buckets::~buckets() {
    // free(random);
}

vector<string> *buckets::put(char *key, int keysize, string value, bool write){
    uint64_t t = clhash(this->random, key, keysize) % this->size;
    if (write) this->bucket[t].push_back(value);
    return &this->bucket[t];
}


bool buckets::load(vector<buckets> &bucket_collector, const char* path) {
    ifstream inFile;
    inFile.open(path, std::ifstream::in|std::ifstream::binary);
    if (!inFile.is_open()) return false;

    char buf[1000];
    int bsize = 0;
    inFile.read((char*)(&bsize), sizeof(bsize));

    for (int b = 0; b < bsize; b++) {
        uint64_t size = 0;
        inFile.read((char*)(&size), sizeof(size));
        buckets bk(size);
        bk.bucket = vector<vector<string> >(bk.size, vector<string>());

        for (int i = 0; i < bk.size; i++) {
            int size = 0;
            inFile.read((char*)(&size), sizeof(size));
            for (int j = 0; j < size; j++) {
                int ssize = 0;
                inFile.read((char*)(&ssize), sizeof(ssize));
                inFile.read(buf, ssize);
                buf[ssize] = '\0';
                bk.bucket[i].push_back(string(buf));
            }
        }

        bucket_collector.push_back(bk);
    }   
    inFile.close();
    return true;
}
bool buckets::save(vector<buckets> &bucket_collector, const char* path) {
    ofstream outFile;
    outFile.open(path, std::ofstream::out|std::ofstream::binary);
    if (!outFile.is_open()) return false;

    int bsize = bucket_collector.size();
    outFile.write((char*)(&bsize), sizeof(bsize));

    for (int b = 0; b < bucket_collector.size(); b++) {
        outFile.write((char*)(&bucket_collector[b].size), sizeof(bucket_collector[b].size));
        
        for (int i = 0; i < bucket_collector[b].bucket.size(); i++) {
            int size = bucket_collector[b].bucket[i].size();
            outFile.write((char*)(&size), sizeof(size));
            for (int j = 0; j < size; j++) {
                int ssize = bucket_collector[b].bucket[i][j].size();
                outFile.write((char*)(&ssize), sizeof(ssize));
                outFile.write(bucket_collector[b].bucket[i][j].c_str(), ssize);
            }
        }
    }

    outFile.close();
    return true;
}


uint64_t buckets::getsize() const {
    return this->size;
}