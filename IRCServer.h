#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "RoomStruct.h"
#include "UserStruct.h"

#ifndef IRC_SERVER
#define IRC_SERVER

#define PASSWORD_FILE "password.txt"

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
bool fileExists(const std::string &name);
std::string sanitizeString(const std::string &input);

class IRCServer {
	// Add any variables you need

	private:
		int open_server_socket(int port);

	public:
		void initialize();
		bool checkPassword(int fd, const std::string &user, const std::string &password);
		void processRequest(int socket);
		void addUser(int fd, const std::string &user, const std::string &password, const std::string &args);
		void createRoom(int fd, const std::string &user, const std::string &password, const std::string &args);
		void enterRoom(int fd, const std::string &user, const std::string &password, const std::string &args);
		void leaveRoom(int fd, const std::string &user, const std::string &password, const std::string &args);
		void listRooms(int fd, const std::string &user, const std::string &password, const std::string &args);
		void sendMessage(int fd, const std::string &user, const std::string &password, const std::string &args);
		void getMessages(int fd, const std::string &user, const std::string &password, const std::string &args);
		void getUsersInRoom(int fd, const std::string &user, const std::string &password, const std::string &args);
		void getAllUsers(int fd, const std::string &user, const std::string &password, const std::string &args);
		void runServer(int port);

		RoomStruct rooms;
		UserStruct users;
};




#endif
