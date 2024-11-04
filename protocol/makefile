.PHONY:all
all:tcpserver tcpclient

tcpserver:Main.cc
	g++ -o $@ $^  -std=c++11
tcpclient:tcpclient.cpp
	g++ -o $@ $^  -std=c++11


.PHONY:clean
clean:
	rm -f tcpserver tcpclient