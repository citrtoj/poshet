#include <string>
#include <iostream>

class Message {
public:
    Message (const std::string& x);
    void Print();
protected:
    std::string msg;
};