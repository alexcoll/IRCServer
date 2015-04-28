#include "UserStruct.h"

//
// Initialize a linked list
//
UserStruct::UserStruct() {
    this->users.head = NULL;
}


//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void UserStruct::print() {
    UserListNode * e = this->users.head;;

    if (this->users.head == NULL) {
        std::cout << "{EMPTY}\n";
        return;
    }

    std::cout << "{";

    while (e != NULL) {
            std::cout << e->name;
        e = e->next;
        if (e != NULL) {
            std::cout << ", ";
        }
    }
    std::cout << "}\n";
}


//
// Appends a new node with this value at the beginning of the list
//
void UserStruct::new_user(const std::string &user, const std::string &password) {
    // Create new node
    UserListNode * e = new UserListNode;
    e->name = user;
    e->password = password;

    // Add at the beginning of the list
    e->next = this->users.head;
    this->users.head = e;
}


//
// Returns true if the value exists in the list.
//
bool UserStruct::exists(const std::string &user) {
    UserListNode * e = this->users.head;;

    if (this->users.head == NULL) {
        // list is empty
        return false;
    }

    while (e != NULL) {
        if (e->name == user) {
            return true;
        }
        e = e->next;
    }
    return false;
}


//
// It removes the entry with that value in the list.
//
bool UserStruct::remove(const std::string &user) {
    UserListNode * e = this->users.head;
    UserListNode * previous = this->users.head;

    if (this->users.head == NULL) {
        return false;
    }

    // Check first value
    if (e->name == user) {
        this->users.head = e->next;
        return true;
    }

    // Check all other values
    while (e->next != NULL) {
        e = e->next;
        if (e->name == user) {
            previous->next = e->next;
            return true;
        }
        previous = e;
    }
    return false;
}


//
// It returns the number of elements in the list.
//
int UserStruct::count() {
    UserListNode * e = this->users.head;

    if (this->users.head == NULL) {
        return false;
    }

    int count = 0;
    while (e != NULL) {
        e = e->next;
        count++;
    }
    return count;
}


std::string UserStruct::get_password(const std::string &user) {
    UserListNode * e = this->users.head;


    if (this->users.head == NULL) {
        // list is empty
        return NULL;
    }

    while (e != NULL) {
        if (e->name == user) {
            return e->password;
        }
        e = e->next;
    }
    return NULL;
}


std::vector<std::string> UserStruct::get_users() {
    UserListNode * e = this->users.head;
    std::vector<std::string> userList;
    std::vector<std::string> nullvector;

    if (this->users.head == NULL) {
        return nullvector;
    }

    while (e != NULL) {
        userList.push_back(e->name);
        e = e->next;
    }
    return userList;
}

