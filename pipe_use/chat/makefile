.PHONY:all
all:client2 client1
client2:client2.cpp
	g++ client2.cpp -o client2 -std=c++11

client1:client1.cpp
	g++ client1.cpp -o client1 -std=c++11

.PHONY:clean
clean:
	rm -f client1 client2 send.txt reverse.txt
