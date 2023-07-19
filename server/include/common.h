#ifndef SPACE_COMMON_H
#define SPACE_COMMON_H

#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <ctime>
#include <vector>
#include <optional>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <string_view>

constexpr std::size_t rows = 100;
constexpr std::size_t cols = 100;
constexpr std::size_t tick = 100;
constexpr std::size_t max_tick = 1000;


struct pair_hash {
  template <typename T1, typename T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2;
  }
};

template<typename T>
void serialize_value(std::vector<uint8_t>& data, const T& value) {
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(T));
}

template<typename T>
static void serialize_data(std::vector<uint8_t>& data, const T& value, size_t length) {
  const uint8_t* p_value = reinterpret_cast<const uint8_t*>(value.data());
  data.insert(data.end(), p_value, p_value + value.size());
  std::fill_n(std::back_inserter(data), length - value.size(), 0);
}

template<typename Base, typename Derived>
bool is_instance_of(const Derived*) {
    return std::is_base_of<Base, Derived>::value;
}

template<std::size_t LENGTH>
class UniqueIDGenerator {
private:
  std::size_t count = 0;
  std::mt19937 gen;

public:
  UniqueIDGenerator() : gen(std::random_device()()) {}

  std::string operator()() {
    std::uniform_int_distribution<> dis('A', 'Z');

    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << ++count;

    while (ss.str().size() < LENGTH)
      ss << static_cast<char>(dis(gen));

    return ss.str();
  }
};


std::optional<std::tuple<double, double, double>> parse_color(const std::string& color);

std::size_t count_unicode_chars(const std::string& str);

std::string_view retrieve_channel(const std::string& str);

uint64_t parse_string_to_epoch(const std::string& str);

#endif //SPACE_COMMON_H