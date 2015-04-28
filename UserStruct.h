#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>


typedef struct UserListNode {
    std::string name;
    std::string password;
    struct UserListNode * next;
} UserListNode;

typedef struct UserList {
    UserListNode * head;
} UserList;

class UserStruct {
    UserList users;

    public:
        UserStruct();

        void init();
        void print();
        void new_user(const std::string &user, const std::string &password);
        bool exists(const std::string &user);
        bool remove(const std::string &user);
        int count();
        std::string get_password(const std::string &user);
        std::vector<std::string> get_users();
};
