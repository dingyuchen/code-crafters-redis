#pragma once

#include "Storage.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
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

struct SimpleError {
  std::string body;
};

class Handler {
private:
  int _client_socket;
  std::vector<char> msg_buffer{};
  std::shared_ptr<Storage> _storage;
  size_t idx;

  ssize_t read_until(size_t size, std::vector<char> &buf); 

  std::vector<std::string> handle(std::stringstream &ss); 

  void read_delim(std::stringstream &ss); 

  std::vector<std::string> handle_aggregate(std::stringstream &ss); 

  void handle_set_command(std::vector<std::string> &req,
                          std::stringstream &out); 

  void resp(std::stringstream &out, const BulkString &bulk_string); 
  void resp(std::stringstream &out, const SimpleString &simple_string); 
  void resp(std::stringstream &out, const SimpleError &simple_error); 

  std::vector<std::string> handle_bulk_string(std::stringstream &ss); 

public:
  Handler(int client_socket, std::shared_ptr<Storage> storage); 

  void handle(); 
};