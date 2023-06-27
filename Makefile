all: SortTimeStamp clean

SortTimeStamp: obj_dir main.o SortTimeStamp.o
	g++ obj/SortTimeStamp.o obj/main.o /usr/local/lib/libboost_filesystem.a -o SortTimeStamp

obj_dir:
	mkdir obj

main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o

SortTimeStamp.o: src/SortTimeStamp.cpp
	g++ -c src/SortTimeStamp.cpp -o obj/SortTimeStamp.o

clean:
	rm -rf obj

install:
	cp SortTimeStamp /usr/local/bin/