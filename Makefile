all: SortTimeStamp

SortTimeStamp: obj_dir main.o SortTimeStamp.o
	g++ -std=c++14 -lboost_filesystem -O2 obj/SortTimeStamp.o obj/main.o /usr/local/lib/libboost_filesystem.a -o SortTimeStamp

obj_dir:
	mkdir -p obj

main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o

SortTimeStamp.o: src/SortTimeStamp.cpp
	g++ -c src/SortTimeStamp.cpp -o obj/SortTimeStamp.o

clean:
	rm -rf obj
	rm SortTimeStamp

install:
	cp SortTimeStamp /usr/local/bin/