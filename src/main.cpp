#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <ctime>
#include <limits>       // std::numeric_limits
using namespace std;


#include "minhash.hpp"
#include "buckets.hpp"



int main() {
    const char *input_dir = "data/docs"; // input datasets document directory
    const char *test_dir = "data/test";  // test directory
    const char *output_dir = "output";  // test directory
    const char *hasher_dist = "hasher.dat";
    const char *buckets_dist = "buckets.dat";
    bool dirty = false;  // if dirty save hasher and bucketer to dist.

    minhasher hasher = minhasher();
    const int band_num = 16;  // band size
    vector<buckets> buckets_collector;

    clock_t tdataset = clock();
    int datasets_count = 0;

    // load buckets cache from dist.
    if (!buckets::load(buckets_collector, buckets_dist)) {
        buckets_collector = vector<buckets>(band_num, buckets(999999));
        cout<<buckets_collector[0].getsize()<<" buckets every band."<<endl;

        // initial buckets with all documents.
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(input_dir)) != NULL) {
            int datasets_count = 0;
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {
                if (string(ent->d_name).size() < 3) continue;  // jump . ..

                ifstream inFile;
                string path = string(input_dir) + "/" + string(ent->d_name);
                inFile.open(path.c_str(), std::ifstream::in);

                vector<uint64_t> signature = hasher.signature(&inFile);   // Get minhash signature

                inFile.close();

                // compute every band bucket
                int band_row = signature.size() / band_num;
                for (int i = 0; i < band_num; i++) {
                    int keysize = (band_row*(i+1) >= signature.size()) ? signature.size()-band_row*i : band_row;
                    buckets_collector[i].put((char*)(signature.data()+i*band_row), keysize*8, path);         // Fill buckets collector
                }

                datasets_count++;
                cout<<"\r"<<datasets_count;
            }
            closedir(dir);


            tdataset = clock()-tdataset;
            cout<<endl;
            cout<<"total "<<datasets_count<<" files."<<endl;
            cout<<"total "<<((float)tdataset)/CLOCKS_PER_SEC<<" s."<<endl;
            cout<<((float)tdataset)/CLOCKS_PER_SEC/datasets_count<<" s per file."<<endl<<endl;
            dirty = true;
        } else {
            /* could not open directory */
            perror ("could not open directory");
            return EXIT_FAILURE;
        }
    }
    else {
        tdataset = clock()-tdataset;
        cout<<"load buckets from dist taking "<<((float)(tdataset))/CLOCKS_PER_SEC<<" s\n\n";
    }

    // save hasher and bucket to dist
    if (dirty) {
        buckets::save(buckets_collector, buckets_dist);
    }
    
    clock_t tfind = clock();
    DIR *dir;
    struct dirent *ent;
    // initial buckets with all documents.
    if ((dir = opendir(test_dir)) != NULL) {
        /* print all the files and directories within directory */
        int file_count = 0;
        int candidate_count = 0;
        int sim_100 = 0;
        int sim_90 = 0;
        int sim_80 = 0;

        while ((ent = readdir(dir)) != NULL) {
            if (string(ent->d_name).size() < 3) continue;  // jump . ..

            ifstream f1;
            string path = string(test_dir) + "/" + string(ent->d_name);
            f1.open(path.c_str(), std::ifstream::in);

            string rpath = string(output_dir) + "/" + string(ent->d_name);
            ofstream rfile;
            rfile.open(rpath.c_str(), std::ofstream::out);

            vector<uint64_t> signature = hasher.signature(&f1);   // Get minhash signature

            rfile<<file_count<<": hashing file: "<<path<<endl;

            // compute every band bucket for similiar file.
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

            candidate_count += sim_file.size();
            rfile<<"got candidate "<<sim_file.size()<<" file(s) :"<<endl;

            // compute similarity result for every candidate.
            for (auto i = sim_file.begin(); i != sim_file.end(); i++) {
                ifstream f2;
                f2.open(i->first.c_str(), std::ifstream::in);
                float sim_rate = hasher.compute_similarity(&f1, &f2);

                if (sim_rate == 1.0) sim_100++;
                else if (sim_rate > 0.9) sim_90++;
                else if (sim_rate > 0.8) sim_80++;

                rfile<<"    "<<i->first<<": "<<sim_rate*100.0<<"%"<<endl;
                f2.close();
            }

            cout<<"\r"<<file_count;
            f1.close();
            rfile.close();
            file_count++;
        }
        closedir(dir);

        tfind = clock()-tfind;
        cout<<endl;
        cout<<"total "<<file_count<<" files."<<endl;
        if (file_count != 0) cout<<"find average "<<((float)candidate_count)/file_count<<" candidate files."<<endl;
        if (candidate_count != 0) {
            cout<<"find sim=1.0 "<<((float)sim_100)/candidate_count*100<<"% candidate files."<<endl;
            cout<<"find sim>0.9 "<<((float)sim_90)/candidate_count*100<<"% candidate files."<<endl;
            cout<<"find sim>0.8 "<<((float)sim_80)/candidate_count*100<<"% candidate files."<<endl;
        }
        else cout<<"0 candidate file.\n";
        cout<<"total "<<((float)tfind)/CLOCKS_PER_SEC<<" s."<<endl;
        if (file_count != 0) cout<<((float)tfind)/CLOCKS_PER_SEC/file_count<<" s per file."<<endl<<endl;
    } else {
        /* could not open directory */
        perror ("could not open directory");
        return EXIT_FAILURE;
    }
    
    
    return 0;
}