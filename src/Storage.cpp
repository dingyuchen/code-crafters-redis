#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <folly/concurrency/ConcurrentHashMap.h>
#include <utility>
#include <vector>

class Storage {
private:
    folly::ConcurrentHashMap<std::string, std::string> map;

public:
    bool set(const std::string& key, const std::string& value) {
        auto res = map.emplace(key, value);
        return res.second;
    }
    
    std::pair<bool, std::string> get(const std::string& key) {
        auto it = map.find(key);
        if (it == map.cend()) {
            return {false, ""};
        }
        return {true, it->second};
    }
};