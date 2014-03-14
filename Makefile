all: utf8encode

utf8encode: main.o
	g++ -O2 main.o -o utf8encode
main.o: main.cpp
	g++ -O2 -c main.cpp -o main.o

clean:
	rm -rf *.o utf8encode
