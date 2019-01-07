#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <ctime>
using namespace std;


#include "minhash.hpp"
#include "buckets.hpp"



int main() {
    const char *input_dir = "data/docs"; // input datasets document directory
    const char *test_dir = "data/test";  // test directory
    const char *hasher_dist = "hasher.dat";
    const char *buckets_dist = "buckets.dat";
    bool dirty = false;  // if dirty save hasher and bucketer to dist.

    minhasher hasher = minhasher();

    const int band_num = 16;  // band size
    vector<buckets> buckets_collector;

    clock_t t = clock();
    // load bucket cache from dist.
    if (!buckets::load(buckets_collector, buckets_dist)) {
        buckets_collector = vector<buckets>(band_num, buckets(1000000));
        t = clock();

        // initial buckets with all documents.
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(input_dir)) != NULL) {
            int file_count = 0;
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {
                if (string(ent->d_name).size() < 3) continue;  // jump . ..
                ifstream inFile;
                string path = string(input_dir) + "/" + string(ent->d_name);
                inFile.open(path.c_str(), std::ifstream::in);
                vector<uint64_t> signature = hasher.signature(&inFile);   // Get minhash signature
                inFile.close();

                int band_row = signature.size() / band_num;

                for (int i = 0; i < band_num; i++) {
                    int keysize = (band_row*(i+1) >= signature.size()) ? signature.size()-band_row*i : band_row;
                    buckets_collector[i].put((char*)(signature.data()+i*band_row), keysize*8, path);         // Fill buckets collector
                }

                file_count++;
                cout<<"\r"<<file_count;
            }
            closedir(dir);
            cout<<endl;

            t = clock()-t;
            cout<<"total "<<file_count<<" files."<<endl;
            cout<<"total "<<((float)t)/CLOCKS_PER_SEC<<" s."<<endl;
            cout<<((float)t)/CLOCKS_PER_SEC/file_count<<" s per file."<<endl<<endl;

            dirty = true;
        } else {
            /* could not open directory */
            perror ("could not open directory");
            return EXIT_FAILURE;
        }
    }
    else {
        cout<<"Load buckets from dist taking "<<((float)(clock()-t))/CLOCKS_PER_SEC<<" s\n\n";
    }


    // save hasher and bucket to dist
    if (dirty) {
        buckets::save(buckets_collector, buckets_dist);
    }
    
    t = clock();
    DIR *dir;
    struct dirent *ent;
    // initial buckets with all documents.
    if ((dir = opendir(test_dir)) != NULL) {
        /* print all the files and directories within directory */
        int file_count = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (string(ent->d_name).size() < 3) continue;  // jump . ..
            ifstream f1;
            string path = string(test_dir) + "/" + string(ent->d_name);
            f1.open(path.c_str(), std::ifstream::in);
            vector<uint64_t> signature = hasher.signature(&f1);   // Get minhash signature
            cout<<file_count<<": Finding file: "<<path<<endl;

            map<string, int> sim_file;
            int band_row = signature.size() / band_num;
            for (int i = 0; i < band_num; i++) {
                int keysize = (band_row*(i+1) >= signature.size()) ? signature.size()-band_row*i : band_row;
                vector<string> *bucket = buckets_collector[i].put((char*)(signature.data()+i*band_row), keysize*8, path, false);         // Fill buckets collector
                for (int s = 0; s < bucket->size(); s++) {
                    if ((*bucket)[s] != path && sim_file.find((*bucket)[s]) == sim_file.end())
                    // if (sim_file.find((*bucket)[s]) == sim_file.end())
                        sim_file.insert(pair<string, int>((*bucket)[s], 1));
                }
            }
            cout<<"Got candidate "<<sim_file.size()<<" file(s) :"<<endl;
            for (auto i = sim_file.begin(); i != sim_file.end(); i++) {
                ifstream f2;
                f2.open(i->first.c_str(), std::ifstream::in);
                float sim_rate = hasher.compute_similarity(&f1, &f2);
                cout<<"    "<<i->first<<": "<<sim_rate*100.0<<"%"<<endl;
                f2.close();
            }
            f1.close();
            file_count++;
            cout<<endl;
        }
        closedir(dir);

        t = clock()-t;
        cout<<"total "<<file_count<<" files."<<endl;
        cout<<"total "<<((float)t)/CLOCKS_PER_SEC<<" s."<<endl;
        cout<<((float)t)/CLOCKS_PER_SEC/file_count<<" s per file."<<endl<<endl;
    } else {
        /* could not open directory */
        perror ("could not open directory");
        return EXIT_FAILURE;
    }

    
    return 0;
}