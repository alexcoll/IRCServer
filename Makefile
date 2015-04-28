
CXX = g++ -fPIC

all: IRCServer

struct: RoomStruct UserStruct

IRCServer: IRCServer.cc RoomStruct.cc UserStruct.cc
	clang -g -std=c++11 -o IRCServer IRCServer.cc RoomStruct.cc UserStruct.cc

RoomSruct: RoomStruct.cc
	g++ -g -std=c++11 -o RoomStruct RoomStruct.cc

UserStruct: UserStruct.cc
	g++ -g -std=c++11 -o UserStruct UserStruct.cc

clean:
	rm -f *.out
	rm -f *.o HashTableVoidTest IRCServer


