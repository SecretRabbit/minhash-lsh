#include "minhash.hpp"

minhasher::minhasher() {
    uint64_t s1 = UINT64_C(0x23a23cf5033c3c81);
    uint64_t s2 = UINT64_C(0xb3816f6a2c68e530);
    int step = 3251;
    for (int i = 0; i < this->hash_num; i++) {
        this->randoms.push_back(get_random_key_for_clhash(s1, s2));
        s1 += step;
        s2 += step;
    }
}

minhasher::~minhasher() {
    // for (int i = 0; i < this->hash_num; i++) {
    //     // free(this->randoms[i]);
    // }
}

vector<uint64_t> minhasher::signature(ifstream *inFile) {
    inFile->clear();
    inFile->seekg(0, ios::beg);
    vector<uint64_t> result(this->hash_num, -1);
    char buf[this->shingle_num+1];

	while(!inFile->eof()) {
        inFile->read(buf, this->shingle_num);
        if (inFile->gcount() < this->shingle_num) break;
        inFile->seekg(-(this->shingle_num-1), ios::cur);
        for (int i = 0; i < this->hash_num; i++) {
            uint64_t h = clhash(this->randoms[i], buf, this->shingle_num);
            if (result[i] > h) {
                result[i] = h;
            }
        }
    }
    return result;
}

float minhasher::compute_similarity(ifstream *f1, ifstream *f2) {
    float total = 0;
    float inter = 0;
    map<string, int> h;

    f1->clear();
    f1->seekg(0, ios::beg);
    f2->clear();
    f2->seekg(0, ios::beg);

    char buf[this->shingle_num+1];
    while(!f1->eof()) {
        f1->read(buf, this->shingle_num);
        if (f1->gcount() < this->shingle_num) break;
        f1->seekg(-(this->shingle_num-1), ios::cur);

        if (h.find(string(buf)) == h.end()) {
            h.insert(pair<string, int>(string(buf), 1));
            total++;
        }
    }

    while(!f2->eof()) {
        f2->read(buf, this->shingle_num);
        if (f2->gcount() < this->shingle_num) break;
        f2->seekg(-(this->shingle_num-1), ios::cur);

        auto iter = h.find(string(buf));
        if (iter == h.end()) {
            h.insert(pair<string, int>(string(buf), 2));
            total++;
        } else {
            if (iter->second == 1) {
                inter++;
                iter->second = 3;
            }
        }
    }
    return inter/total;
}

/*
bool minhasher::load(const char *path) {
    ifstream inFile;
    inFile.open(path, std::ifstream::in|std::ifstream::binary);
    if (!inFile.is_open()) return false;

    inFile.read((char*)(&this->hash_num), sizeof(this->hash_num));
    inFile.read((char*)(&this->shingle_num), sizeof(this->shingle_num));

    cout<<"hash_num: "<<this->hash_num<<endl;
    cout<<"shingle_num: "<<this->shingle_num<<endl;

    cout<<((uint64_t*)this->randoms[0])[0]<<" "<<((uint64_t*)this->randoms[1])[0]<<endl;
    cout<<((uint64_t*)this->randoms[2])[0]<<" "<<((uint64_t*)this->randoms[3])[0]<<endl;
    for (int i = 0; i < this->hash_num;i++) {
        inFile.read((char*)(&this->randoms[i]), RANDOM_BYTES_NEEDED_FOR_CLHASH);
    }
    cout<<((uint64_t*)this->randoms[0])[0]<<" "<<((uint64_t*)this->randoms[1])[0]<<endl;
    cout<<((uint64_t*)this->randoms[2])[0]<<" "<<((uint64_t*)this->randoms[3])[0]<<endl;

    inFile.close();
    return true;
}
bool minhasher::save(const char *path) {
    ofstream outFile;
    outFile.open(path, std::ofstream::out|std::ofstream::binary);
    if (!outFile.is_open()) return false;
    
    outFile.write((char*)(&this->hash_num), sizeof(this->hash_num));
    outFile.write((char*)(&this->shingle_num), sizeof(this->shingle_num));

    for (int i = 0; i < this->hash_num; i++) {
        outFile.write((char*)(&this->randoms[i]), RANDOM_BYTES_NEEDED_FOR_CLHASH);
    }
    cout<<((uint64_t*)this->randoms[0])[0]<<" "<<((uint64_t*)this->randoms[1])[0]<<endl;
    cout<<((uint64_t*)this->randoms[2])[0]<<" "<<((uint64_t*)this->randoms[3])[0]<<endl;

    outFile.close();
    return true;
}
*/