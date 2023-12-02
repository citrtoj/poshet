#include <iostream>
#include "GUIComponents.hpp"
#include "Utils.hpp"
#include "Pop3Connection.hpp"

int main() {
    Pop3Connection connection("pop3.localhost", "110");
    connection.connectToPop3Server();
    std::tuple<int, std::string> tmp = connection.execCommand("user john");
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    tmp = connection.execCommand("pass john");
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    tmp = connection.execCommand("list");
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);

}