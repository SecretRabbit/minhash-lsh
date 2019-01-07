#ifndef BUCKET_HPP
#define BUCKET_HPP
#include <vector>
using namespace std;
#include "clhash.h"

class buckets {
    private:
        vector<vector<string>> bucket;
        uint64_t size;
        // clhasher h;
        void* random;
    public:
        buckets(uint64_t size);
        ~buckets();
        vector<string> *put(char *key, int keysize, string value, bool write=true);
        static bool load(vector<buckets> &bucket_collector, const char *path);
        static bool save(vector<buckets> &bucket_collector, const char *path);
};

#endif