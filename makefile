
compile: src/clhash/clhash.o
	g++ -std=c++11 -I./src/clhash/include src/clhash/clhash.o src/buckets.cpp src/minhash.cpp src/main.cpp -o main

src/clhash/clhash.o:
	cd src/clhash && make && cd ..