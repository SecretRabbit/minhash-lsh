import json
import argparse
import os
from tqdm import tqdm
import numpy as np

""" Convert News Information Retrieval (NewsIR’16) Dataset from json data to .txt file
for testing document searching. Also put 10 test file to test dir.
"""
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_json", type=str, required=True);
    parser.add_argument("--output_dir", type=str, required=True);
    parser.add_argument("--test_dir", type=str, required=True);
    parser.add_argument("--test_num", type=int, default=1000);
    args = parser.parse_args()

    data_json = args.data_json
    output_dir = args.output_dir
    test_dir = args.test_dir

    with open(data_json, 'r') as f:
        lines = f.readlines()
        ridxs = np.random.randint(0, len(lines), args.test_num)
        i = 0

        for l in tqdm(lines):
            news_article = json.loads(l)

            output_doc = os.path.join(output_dir, news_article['id']+'.txt')

            with open(output_doc, 'w') as of:
                of.write(news_article['title']+"\n")
                of.write(news_article['content'])

            # random choice test file
            if i in ridxs:
                output_doc = os.path.join(test_dir, news_article['id']+'.txt')
                with open(output_doc, 'w') as of:
                    of.write(news_article['title']+"\n")
                    of.write(news_article['content'])
        
            i += 1
            