#include "Message.hpp"

Message::Message(const std::string& x) : msg(x) {}

void Message::Print() {
    std::cout << msg;
}