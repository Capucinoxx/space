#ifndef SPACE_UTILS_H
#define SPACE_UTILS_H

#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <ctime>
#include <vector>
#include <optional>
#include <algorithm>

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

std::optional<std::tuple<double, double, double>> parse_color(const std::string& color) {
  std::istringstream iss(color);
  std::vector<double> hsl;
  double value;

  while (iss >> value) {
    std::cout << value << std::endl;
    hsl.push_back(value);
  }
    

  if (hsl.size() != 3)
    return std::nullopt;

  return std::make_tuple(hsl[0], hsl[1], hsl[2]);
}

std::size_t count_unicode_chars(const std::string& str) {
  std::size_t count = 0;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if ((str[i] & 0xC0) != 0x80)
      ++count;
  }

  return count;
}

#endif //SPACE_UTILS_H
