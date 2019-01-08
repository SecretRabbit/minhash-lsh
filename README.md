# Minhash LSH

A program implements Minhash LSH on 1million articles.

Algorithm follows the minhash LSH discrible from [here](http://infolab.stanford.edu/~ullman/mmds/ch3.pdf).

Thank for the hash solution from [clhash](https://github.com/lemire/clhash).

## Datasets

We test our minhash on [Recent Trends in News Information Retrieval (NewsIR’16)](https://research.signal-ai.com/newsir16/signal-dataset.html) dataset and random choose 1000 article from it to test minhash LSH searching result.

You can download and unpackage the dataset to datasets directory. Then, run the following command to convert json data to 1 million documents.

```shell
cd xxx/minhash-lsh
mkdir data && mkdir data/docs && mkdir data/test
bash json2doc.sh
```

Then, all 1 million documents and 1000 test documents will be prepared under `data/docs` and `data/test`.



## Compile and Test

Under `minhash-lsh` document and use `make` to compile the c++ projects.

Before run the program just make directory `output` for output searching results.

```
mkdir output
```

Run `./main` to minhash all 1 million datasets and test 1000 test data. All results are in `output` directory.

**Note:** When the first time to run the program, it will take about more than **3 hours** on minhash all 1 millions data! This is because our program is not optimized to run on gpu (It only run on cpu which make it so slow). But after first time you make minhash on dataset, it will save the bucket to dist `buckets.dat`. So, the second time run the program will load bucket from dist which will be fast.



## Result

```
total 1000 files.
find average 21.66 candidate files.
find sim=1.0 6.63435% candidate files.
find sim>0.9 2.63158% candidate files.
find sim>0.8 12.3684% candidate files.
total 172.825 s.
0.172825 s per file.
```

We find that our algorithm can find candidate files quickly. In fact we test similarity between candidate file and test file and find that **there are many low similarity file (false positie) in candidates**. We think about that may be it is the result of less number of buckests.

In the future, we may be can optimize the program on gpu and use better solution to random construct hash function.