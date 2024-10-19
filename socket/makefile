.PHONY:all
all:udpserver udpclient

udpserver:Main.cc
	g++ -o $@ $^ -std=c++11
udpclient:socket_client.cc
	g++ -o $@ $^ -lpthread -std=c++11


.PHONY:clean
clean:
	rm -f udpserver udpclient