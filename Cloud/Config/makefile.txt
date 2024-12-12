#text:code.cpp
#	g++ -o $@ $^  -std=c++11 -lstdc++fs -lpthread -ljsoncpp -L./Lib -lbundle


#.PHONY:clean
#clean:
#	rm -f text 

text:code.cpp
	g++ -o $@ $^ -std=c++11  -I./include  -L./lib  -L/usr/lib64/mysql  -lmysqlclient -lstdc++fs -lpthread -L./Lib -lbundle -ljsoncpp -g


.PHONY:clean
clean:
	rm -f text 