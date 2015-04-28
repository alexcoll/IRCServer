#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>


typedef struct RoomListNode {
    std::string name;
    int msgNum = 0;
    std::vector<std::string> messages;
    std::vector<std::string> users;
    struct RoomListNode * next;
} RoomListNode;

typedef struct RoomList {
    RoomListNode * head;
} RoomList;

class RoomStruct {
    RoomList rooms;

    public:
        RoomStruct();

        void init();
        void print();
        void new_room(const std::string &room);
        bool exists(const std::string &room);
        bool remove_room(const std::string &room);
        int count();
        std::vector<std::string> get_users(const std::string &room);
        std::vector<std::string> get_messages(const std::string &room, const int lastMsgNum);
        std::vector<std::string> get_rooms();
        bool add_user(const std::string &room, const std::string &user);
        bool remove_user(const std::string &room, const std::string &user);
        bool add_message(const std::string &room, const std::string &user, const std::string &message);
        bool in_room(const std::string &room, const std::string &user);
};
