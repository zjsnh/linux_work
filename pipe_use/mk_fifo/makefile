.PHONY : all
all:client server
client:client.cpp
	g++ client.cpp -o client
server:server.cpp
	g++ server.cpp -o server


.PHONY:clean
clean:
	rm -f client
	rm -f server