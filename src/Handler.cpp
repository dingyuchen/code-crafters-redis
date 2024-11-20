#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <vector>

class Handler {
private:
  int _client_socket;
  std::array<std::byte, 1024> buffer{};
  std::vector<char> msg_buffer{};
  size_t idx;

  void read_until(size_t size) {}

  void read_until(std::string delimiter) {}

  void handle_aggregate() {}

  void handle_bulk_string() {}

public:
  Handler(int client_socket) {
    _client_socket = client_socket;
    idx = 0;
  }

  void handle() {
    while (ssize_t size =
               recv(_client_socket, buffer.data(), buffer.size(), 0)) {
      std::stringstream ss{};
      for (size_t i = 0; i < size; i++) {
        ss << static_cast<char>(buffer[i]);
      }
      char b;
      ss >> b;
      if ('*' == b) {
        handle_aggregate();
      } else if ('$' == b) {
        handle_bulk_string();
      }
      std::cout << b << std::endl;

      std::string pong = "+PONG\r\n";
      send(_client_socket, pong.data(), pong.size(), 0);
    }
  }
};