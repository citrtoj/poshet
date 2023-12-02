#include <iostream>
#include "GUIComponents.hpp"
#include "Utils.hpp"
#include "Pop3Connection.hpp"

int main() {
    Pop3Connection connection;
    connection.setHost("localhost");
    connection.setPort("110");
    connection.connectToPop3Server();
    std::tuple<int, std::string> tmp = connection.execCommand("user john");
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    tmp = connection.execCommand("pass john");
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    tmp = connection.execCommand("list", true);
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);

    sleep(4);
    tmp = connection.execCommand("list", true);
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    sleep(1);
    tmp = connection.execCommand("list", true);
    std::cout << std::get<0>(tmp) << " " << std::get<1>(tmp);
    connection.closeConnection();
}