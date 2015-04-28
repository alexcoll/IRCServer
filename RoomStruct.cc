#include "RoomStruct.h"

//
// Initialize a linked list
//
RoomStruct::RoomStruct() {
    this->rooms.head = NULL;
}


//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void RoomStruct::print() {
    RoomListNode * e;

    if (rooms.head == NULL) {
        std::cout << "{EMPTY}\n";
        return;
    }

    std::cout << "{";

    e = rooms.head;
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
void RoomStruct::new_room(const std::string &room) {
    // Create new node
    RoomListNode * e = new RoomListNode;
    e->name = room;
    e->msgNum = 0;

    // Add at the beginning of the list
    e->next = rooms.head;
    rooms.head = e;
}


//
// Returns true if the value exists in the list.
//
bool RoomStruct::exists(const std::string &room) {
    RoomListNode * e;

    if (rooms.head == NULL) {
        // list is empty
        return false;
    }

    e = rooms.head;
    while (e != NULL) {
        if (e->name == room) {
            return true;
        }
        e = e->next;
    }
    return false;
}


//
// It removes the entry with that value in the list.
//
bool RoomStruct::remove_room(const std::string &room) {
    RoomListNode * e = rooms.head;
    RoomListNode * previous = rooms.head;

    if (rooms.head == NULL) {
        return false;
    }

    // Check first value
    if (e->name == room) {
        rooms.head = e->next;
        return true;
    }

    // Check all other values
    while (e->next != NULL) {
        e = e->next;
        if (e->name == room) {
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
int RoomStruct::count() {
    RoomListNode * e = rooms.head;

    if (rooms.head == NULL) {
        return false;
    }

    int count = 0;
    while (e != NULL) {
        e = e->next;
        count++;
    }
    return count;
}


std::vector<std::string> RoomStruct::get_users(const std::string &room) {
    RoomListNode * e = rooms.head;
    std::vector<std::string> nullvector;

    if (rooms.head == NULL) {
        // list is empty
        return nullvector;
    }

    while (e != NULL) {
        if (e->name == room) {
            return e->users;
        }
        e = e->next;
    }
    return nullvector;
}


std::vector<std::string> RoomStruct::get_messages(const std::string &room, const int lastMsgNum) {
    // FIXME

    RoomListNode * e = rooms.head;
    std::vector<std::string> nullvector;
    std::vector<std::string> returnVector;

    if (rooms.head == NULL) {
        // list is empty
        return nullvector;
    }

    while (e != NULL) {
        if (e->name == room) {
            int msgToGet = e->msgNum - lastMsgNum;
            if (msgToGet == 0) {
                returnVector.push_back("NO-NEW-MESSAGESS"); // won't print last s for some unkown reason
                return returnVector;
            } else {
                for (std::vector<int>::size_type i = lastMsgNum; i != e->messages.size(); i++) {
                    returnVector.push_back(e->messages.at(i));
                }
                return returnVector;
            }
        }
        e = e->next;
    }
    return nullvector;
}


bool RoomStruct::add_user(const std::string &room, const std::string &user) {
    RoomListNode * e = rooms.head;

    if (rooms.head == NULL) {
        // list is empty
        return false;
    }

    while (e != NULL) {
        if (e->name == room) {
            for (std::vector<int>::size_type i = 0; i != e->users.size(); i++) {
                if (e->users[i] == user) {
                    return true;
                }
            }
            e->users.push_back(user);
            return true;
        }
        e = e->next;
    }
    return false;
}

bool RoomStruct::remove_user(const std::string &room, const std::string &user) {
    RoomListNode * e = rooms.head;

    if (rooms.head == NULL) {
        // list is empty
        return false;
    }

    while (e != NULL) {
        if (e->name == room) {
            int n = 0;
            for (std::vector<int>::size_type i = 0; i != e->users.size(); i++) {
                if (e->users[i] == user) {
                    n = i;
                    break;
                }
            }
            e->users.erase(e->users.begin() + n);
            e->users.shrink_to_fit();
            return true;
        }
        e = e->next;
    }
    return false;
}


bool RoomStruct::add_message(const std::string &room, const std::string &user, const std::string &message) {
    RoomListNode * e = rooms.head;
    std::string msgString = "";

    if (rooms.head == NULL) {
        // list is empty
        return false;
    }

    while (e != NULL) {
        if (e->name == room) {
            if (e->messages.size() > 100) {
                e->messages.erase(e->messages.begin());
                e->messages.shrink_to_fit();
            }
            std::string temp;
            char * numstr = (char*) malloc(21 * sizeof(char)); // enough to hold all numbers up to 64-bits
            sprintf(numstr, "%d", e->msgNum);
            temp = msgString + numstr;
            msgString = temp + " " + user + " " + message;
            e->messages.push_back(msgString);
            std::cout << "Message added=" << e->messages.back() << "\n";
            e->msgNum++;
            return true;
        }
        e = e->next;
    }
    return false;
}


bool RoomStruct::in_room(const std::string &room, const std::string &user) {
    RoomListNode * e = rooms.head;

    if (rooms.head == NULL) {
        // list is empty
        return false;
    }

    while (e != NULL) {
        if (e->name == room) {
            for (std::vector<int>::size_type i = 0; i != e->users.size(); i++) {
                if (e->users[i] == user) {
                    return true;
                }
            }
        }
        e = e->next;
    }
    return false;
}


std::vector<std::string> RoomStruct::get_rooms() {
    RoomListNode * e = this->rooms.head;
    std::vector<std::string> nullvector;
    std::vector<std::string> roomList;

    if (this->rooms.head == NULL) {
        return nullvector;
    }

    while (e != NULL) {
        roomList.push_back(e->name);
        e = e->next;
    }
    return roomList;
}
