all: test

test:
	g++ skiplist.hpp test.cpp -o main

debug:
	g++ -g skiplist.hpp test.cpp -o main
clean:
	rm main
