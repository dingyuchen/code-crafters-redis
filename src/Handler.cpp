#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

struct SimpleString {
  std::string body;
};

struct BulkString {
  std::string body;
};

class Handler {
private:
  int _client_socket;
  std::vector<char> msg_buffer{};
  size_t idx;

  ssize_t read_until(size_t size, std::vector<char> &buf) {
    return recv(size, buf.data(), buf.size(), 0);
  }

  std::vector<std::string> handle(std::stringstream &ss) {
    char b;
    ss.read(&b, 1);
    if ('*' == b) {
      return handle_aggregate(ss);
    } else if ('$' == b) {
      return handle_bulk_string(ss);
    }
    return {};
  }

  void read_delim(std::stringstream &ss) {
    char c[2];
    ss.read(c, 2);
  }

  std::vector<std::string> handle_aggregate(std::stringstream &ss) {
    int size;
    ss >> size;
    read_delim(ss);
    std::vector<std::string> strings;
    for (size_t i = 0; i < size; i++) {
      auto res = handle(ss);
      strings.insert(strings.end(), res.begin(), res.end());
    }
    if (strings.empty()) {
      return strings;
    }

    std::stringstream out;
    if (strings.front() == "ECHO") {
      resp(out, BulkString{strings.back()});
    } else {
      resp(out, SimpleString{"PONG"});
    }

    std::string res = out.str();
    send(_client_socket, res.data(), res.size(), 0);
    return strings;
  }

  // to overload between bulkstring and simple strings
  void resp(std::stringstream &out, const BulkString &bulk_string) {
    out << '$';
    out << bulk_string.body.size() << "\r\n";
    out << bulk_string.body;
    out << "\r\n";
  }

  void resp(std::stringstream &out, const SimpleString &simple_string) {
    out << '+';
    out << simple_string.body << "\r\n";
  }

  std::vector<std::string> handle_bulk_string(std::stringstream &ss) {
    int size;
    ss >> size;
    std::string word;
    word.resize(size);
    read_delim(ss);
    ss.read(word.data(), size);
    read_delim(ss);
    // std::cout << "received_string: " << word << std::endl;
    return {word};
  }

public:
  Handler(int client_socket) {
    _client_socket = client_socket;
    idx = 0;
  }

  void handle() {
    std::array<std::byte, 1024> buffer{};
    while (size_t size =
               recv(_client_socket, buffer.data(), buffer.size(), 0)) {
      std::stringstream ss{};
      for (size_t i = 0; i < size; i++) {
        ss << static_cast<char>(buffer[i]);
      }
      handle(ss);
    }
  }
};