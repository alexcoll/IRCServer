#include "IRCServer.h"

std::string usage =
"                                                               \n"
"IRCServer:                                                     \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"\tIRCServer <port>                                             \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

int QueueLength = 5;


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


bool fileExists(const std::string &name) {
	struct stat buffer;
  	return (stat (name.c_str(), &buffer) == 0);
}


std::string sanitizeString(const std::string &input) {
	std::string output;
	int j = 0;
	for (int i = 0; i < input.length(); i++) {
		// std::cout << "Checking character=" << input[i] << "\n";
		// Allow letters
		if ((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')) {
			output.push_back(input[i]);
			j++;
			continue;
		}
		// Allow numbers
		if (input[i] >= '0' && input[i] <= '9') {
			output.push_back(input[i]);
			j++;
			continue;
		}
		// Allow special suymbols:
		if (input[i] == '.' || input[i] == '/' || input[i] == '+' || input[i] == '=' || input[i] == ' ' || input[i] == '(' || input[i] == ')' || input[i] == '[' || input[i] == ']' || input[i] == '{' || input[i] == '}' || input[i] == '-' || input[i] == '*' || input[i] == '_' || input[i] == '!' || input[i] == '#' || input[i] == '$' || input[i] == '@' || input[i] == '%' || input[i] == '&') {
			output.push_back(input[i]);
			j++;
			continue;
		}
	}
	output.shrink_to_fit();
	return output;
}


int IRCServer::open_server_socket(int port) {
	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress;
	memset(&serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);

	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if (masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1;
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR,
			     (char *) &optval, sizeof(int));

	// Bind the socket to the IP address and port
	int error = bind(masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if (error) {
		perror("bind");
		exit(-1);
	}

	// Put socket in listening mode and set the
	// size of the queue of unprocessed connections
	error = listen(masterSocket, QueueLength);
	if (error) {
		perror("listen");
		exit(-1);
	}

	return masterSocket;
}


void IRCServer::runServer(int port) {
	int masterSocket = open_server_socket(port);

	initialize();

	while (1) {

		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof(clientIPAddress);
		int slaveSocket = accept(masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);

		if (slaveSocket < 0) {
			perror("accept");
			exit(-1);
		}

		// Process request.
		processRequest(slaveSocket);
	}
}


int main(int argc, char ** argv) {
	// Print usage if not enough arguments
	if (argc < 2) {
		fprintf(stderr, "%s", usage.c_str());
		exit(-1);
	}

	// Get the port from the arguments
	int port = atoi(argv[1]);

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
}


/*
Commands:
  Commands are started by the client.

  Request: ADD-USER <USER> <PASSWD>\r\n
  Answer: OK\r\n or DENIED\r\n

  REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
  Answer: USER1\r\n
           USER2\r\n
           ...
           \r\n

  REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
  Answer: OK\n or DENIED\r\n

  Request: LIST-ROOMS <USER> <PASSWD>\r\n
  Answer: room1\r\n
          room2\r\n
          ...
          \r\n

  Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
  Answer: OK\n or DENIED\r\n

  Request: LEAVE-ROOM <USER> <PASSWD>\r\n
  Answer: OK\n or DENIED\r\n

  Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
  Answer: OK\n or DENIED\n

  Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
  Answer: MSGNUM1 USER1 MESSAGE1\r\n
          MSGNUM2 USER2 MESSAGE2\r\n
          MSGNUM3 USER2 MESSAGE2\r\n
          ...\r\n
          \r\n

   REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
   Answer: USER1\r\n
           USER2\r\n
           ...
           \r\n
*/


void IRCServer::processRequest(int fd) {
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[MaxCommandLine + 1];
	int commandLineLength = 0;
	int n;

	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;

	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command std::string is full.
	while (commandLineLength < MaxCommandLine && read(fd, &newChar, 1) > 0) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}

		commandLine[commandLineLength] = newChar;
		commandLineLength++;
		prevChar = newChar;
	}

	// Add null character at the end of the char *
	// Eliminate last \r
	commandLineLength--;
        commandLine[commandLineLength] = 0;

	std::cout << "RECEIVED: " << commandLine << "\n";

	// Sanitize the input
	std::string sanitizedCmdLine = sanitizeString(commandLine);

	// std::cout << "SANITIZED RECIEVED: " << sanitizedCmdLine << "\n";

	std::string command;
	std::string user;
	std::string password;
	std::string args = "";

	std::vector<std::string> argList = split(sanitizedCmdLine, ' ');
	if (argList.size() >= 3) {
		// !!!!!!! watch for empty tokens and the "\r\n" characters at the end of the string !!!!!!
		command = argList[0];
		user = argList[1];
		password = argList[2];
		if (argList.size() > 3) {
			for (std::vector<int>::size_type i = 3; i != argList.size(); i++) {
				args = args + (argList[i] + " ");
	        }
	        if (args.at(args.length() - 1) == ' ') {
	        	args.resize(args.length() - 1);
	        }
	    }
	} else {
		const char * msg =  "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "agument error\r\n";
		close(fd);
		return;
	}

	std::cout << "command=" << command << "\n";
	std::cout << "user=" << user << "\n";
	std::cout << "password=" << password << "\n";
	std::cout << "args=" << args << "\n";


	if (command == "ADD-USER") {
		addUser(fd, user, password, args);
	} else if (this->checkPassword(fd, user, password)) {
		if (command == "CREATE-ROOM") {
			createRoom(fd, user, password, args);
		} else if (command == "ENTER-ROOM") {
			enterRoom(fd, user, password, args);
		} else if (command == "LEAVE-ROOM") {
			leaveRoom(fd, user, password, args);
		} else if (command == "LIST-ROOMS") {
			listRooms(fd, user, password, args);
		} else if (command == "SEND-MESSAGE") {
			sendMessage(fd, user, password, args);
		} else if (command == "GET-MESSAGES") {
			getMessages(fd, user, password, args);
		} else if (command == "GET-USERS-IN-ROOM") {
			getUsersInRoom(fd, user, password, args);
		} else if (command == "GET-ALL-USERS") {
			getAllUsers(fd, user, password, args);
		} else {
			const char * msg =  "UNKNOWN COMMAND\r\n";
			write(fd, msg, strlen(msg));
			std::cout << "ERROR (Unkown command)\n";
		}
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (Wrong password)\n";
	}

	close(fd);
}


void IRCServer::initialize() {
	// Create password file
	if (!fileExists(PASSWORD_FILE)) {
		std::ofstream createFile;
		createFile.open(PASSWORD_FILE);
		createFile.close();
		std::cout << "Password file created\n";
	}
	// Open password file
	std::ifstream passFile;
	passFile.open(PASSWORD_FILE);
	if (passFile.is_open()) {
		std::string line;
		std::cout << "Reading password file\n";
		while (getline(passFile, line)) {
			std::vector<std::string> tempList;
			tempList = split(line, ',');
			if (tempList.size() == 2) {
				std::string user = tempList.at(0);
				std::string password = tempList.at(1);
				users.new_user(user, password);
				std::cout << "Added user " << user << "\n";
			}
		}
		std::cout << "Done reading password file\n";
	} else {
		std::cout << "ERROR, cant read password file\n";
		exit(1);
	}
	passFile.close();

	// Initialize users/rooms
	RoomStruct * rooms = new RoomStruct;
	UserStruct * users = new UserStruct;
}


bool IRCServer::checkPassword(int fd, const std::string &user, const std::string &password) {
	if (users.exists(user)) {
		if (users.get_password(user) == password) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
	return false;
}


void IRCServer::addUser(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;

	// check input (no spaces or commas or semicolons)
	size_t none = std::string::npos;
	if ((user.find(",") != none) || (user.find(" ") != none) || (user.find(";") != none)) {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		return;
	}

	if ((password.find(",") != none) || (password.find(" ") != none) || (password.find(";") != none)) {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		return;
	}

	if (!users.exists(user)) {
		users.new_user(user, password);
		// write to passwords.txt
		std::ofstream passFile;
		passFile.open(PASSWORD_FILE, std::ios::in | std::ios::app);
		if (passFile.is_open()) {
			passFile << user << ',' << password << "\n";
			passFile.close();
		} else {
			// this shouldnt happen
			msg = "DENIED\r\n";
			write(fd, msg, strlen(msg));
			std::cout << "ERROR (couldn't open file; this error shouldn't happen)";
			return;
		}
		msg = "OK\r\n";
		write(fd, msg, strlen(msg));
		return;
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}


void IRCServer::createRoom(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::string room;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() == 1) {
		room = arguments[0];
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (!rooms.exists(room)) {
		rooms.new_room(room);
		msg = "OK\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "Room \"" << room << "\" created\n";
		return;
	} else {
		std::cout << "ERROR (Room already exists)\n";
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (Room already exists)\n";
		return;
	}
}


void IRCServer::enterRoom(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::string room;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() == 1) {
		room = arguments[0];
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (rooms.exists(args)) {
		if (rooms.in_room(room, user)) {
			msg = "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		} else if (rooms.add_user(room, user)) {
			msg = "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		} else {
			msg = "DENIED\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	} else {
		msg = "ERROR (No room)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (No room)\n";
		return;
	}
}


void IRCServer::leaveRoom(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::string room;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() == 1) {
		room = arguments[0];
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (rooms.in_room(room, user)) {
		if (rooms.remove_user(room, user)) {
			msg = "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		} else {
			msg = "DENIED\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	} else {
		msg = "ERROR (No user in room)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (No user in room)\n";
		return;
	}
}


void IRCServer::listRooms(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::vector<std::string> nullvector;
	std::vector<std::string> roomList;

	roomList = rooms.get_rooms();
	if (roomList != nullvector) {
		std::sort(roomList.begin(), roomList.end());
		std::cout << "List of Rooms:\n";
		for (std::vector<int>::size_type i = 0; i != roomList.size(); i++) {
			std::cout << roomList[i] << "\n";
			msg = roomList[i].c_str();
			write(fd, msg, strlen(msg));
			msg = "\r\n";
			write(fd, msg, strlen(msg));
        }
	} else {
		msg = "OK\r\n";
		std::cout << "NO-ROOMS\n";
		write(fd, msg, strlen(msg));
		return;
	}
}


void IRCServer::sendMessage(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::string room;
	std::string message;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() >= 2) {
		room = arguments.at(0);
		for (std::vector<int>::size_type i = 1; i != arguments.size(); i++) {
			message += arguments.at(i) + " ";
        }
	} else {
		msg = "ERROR (user not in room)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (rooms.in_room(room, user)) {
		if (rooms.add_message(room, user, message)) {
			msg = "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		} else {
			msg = "DENIED\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	} else {
		msg = "ERROR (user not in room)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (user not in room)\n";
		return;
	}
}


void IRCServer::getMessages(int fd, const std::string &user, const std::string &password, const std::string &args) {
	// FIXME
	const char * msg;
	int lastMsgNum;
	std::string room;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() == 2) {
		lastMsgNum = atoi(arguments.at(0).c_str());
		if (lastMsgNum < 0 || lastMsgNum > 99) {
			msg = "NO-NEW-MESSAGES\r\n";
			write(fd, msg, strlen(msg));
			std::cout << "ERROR (arguments)\n";
			return;
		}
		room = arguments.at(1);
	} else {
		msg = "ERROR (arguments)\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (rooms.in_room(room, user)) {
		std::vector<std::string> nullvector;
		std::vector<std::string> messages;

		messages = rooms.get_messages(room, lastMsgNum);
		if (messages == nullvector) {
			msg = "NO-NEW-MESSAGES MEOW\r\n";
			std::cout << "NO-NEW-MESSAGES\n";
			write(fd, msg, strlen(msg));
			return;
		} else {
			for (std::vector<int>::size_type i = 0; i != messages.size(); i++) {
				msg = messages.at(i).c_str();
				// This write command caused problems, using strlen(msg)) would brek test cases
				write(fd, msg, strlen(msg + 1));
				if (messages.at(i) != "NO-NEW-MESSAGESS") {
					msg = "\r\n";
					write(fd, msg, strlen(msg));
				}
        	}
        	msg = "\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	} else {
		msg = "ERROR (User not in room)\r\n";
		std::cout << "ERROR (User not in room)\n";
		write(fd, msg, strlen(msg));
		return;
	}
}


void IRCServer::getUsersInRoom(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::string room;
	std::vector<std::string> arguments;

	arguments = split(args, ' ');
	if (arguments.size() == 1) {
		room = arguments[0];
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		std::cout << "ERROR (arguments)\n";
		return;
	}

	if (rooms.exists(room)) {
		std::vector<std::string> userList;
		std::vector<std::string> nullvector;

		userList = rooms.get_users(room);
		if (userList == nullvector) {
			// List is empty
			msg = "\r\n"; // respnd with only a newline if userList is empty
			write(fd, msg, strlen(msg));
			return;
		} else {
			// Print a sorted list of users currently in room
			std::sort(userList.begin(), userList.end());
			for (std::vector<int>::size_type i = 0; i != userList.size(); i++) {
				msg = userList[i].c_str();
				write(fd, msg, strlen(msg));
				msg = "\r\n";
				write(fd, msg, strlen(msg));
        	}
        	msg = "\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	} else {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}


void IRCServer::getAllUsers(int fd, const std::string &user, const std::string &password, const std::string &args) {
	const char * msg;
	std::vector<std::string> userList;
	std::vector<std::string> nullvector;

	userList = users.get_users();
	if (userList == nullvector) {
		msg = "DENIED\r\n";
		write(fd, msg, strlen(msg));
		return;
	} else {
		// Print a sorted list of all existing users
		std::sort(userList.begin(), userList.end());
		for (std::vector<int>::size_type i = 0; i != userList.size(); i++) {
			msg = userList[i].c_str();
			write(fd, msg, strlen(msg));
			msg = "\r\n";
			write(fd, msg, strlen(msg));
    	}
    	msg = "\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

