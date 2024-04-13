#include <iostream>

#include "config.hpp"

int main() {
  server my_server;
  if (my_server.parse_config("default1.config", my_server)) {
    // std::cout << "Configuration parsed successfully." << std::endl;
    my_server.print();
  } else {
    std::cout << "Failed to parse configuration." << std::endl;
  }
  return 0;
}