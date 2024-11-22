#pragma once

#include <chrono>
#include <folly/concurrency/ConcurrentHashMap.h>
#include <iostream>
#include <string>
#include <vector>

struct Value {
  std::string body;
  std::optional<std::chrono::time_point<std::chrono::steady_clock>> expire_time;

  Value(
      const std::string &body,
      std::optional<std::chrono::time_point<std::chrono::steady_clock>> expiry)
      : body(body), expire_time(expiry) {}
};

class Storage {
private:
  folly::ConcurrentHashMap<std::string, Value> map;

public:
  bool set(const std::string &key, const std::string &value) {
    auto res = map.emplace(key, Value{value, {}});
    return res.second;
  }

  bool set(const std::string &key, const std::string &value, uint64_t ms) {
    const auto start{std::chrono::steady_clock::now()};
    const auto expire = start + std::chrono::milliseconds(ms);
    auto res = map.try_emplace(key, value, expire);
    if (!res.second) {
      // will update the existing value
      res = map.insert_or_assign(key, Value{value, expire});
    }
    return res.second;
  }

  std::pair<bool, std::string> get(const std::string &key) {
    auto it = map.find(key);
    if (it == map.cend()) {
      return {false, ""};
    }

    bool expired = it->second.expire_time
                       .transform([](const auto &time) {
                         return time < std::chrono::steady_clock::now();
                       })
                       .value_or(false);
    if (expired) {
      map.erase(key);
      return {false, ""};
    }
    return {true, it->second.body};
  }
};