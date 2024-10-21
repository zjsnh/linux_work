.PHONY:all
all:tcpserver tcpclient

tcpserver:Main.cc
	g++ -o $@ $^ -lpthread -std=c++11
tcpclient:socket_client.cc
	g++ -o $@ $^  -std=c++11


.PHONY:clean
clean:
	rm -f tcpserver tcpclient