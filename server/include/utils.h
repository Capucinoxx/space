#ifndef SPACE_UTILS_H
#define SPACE_UTILS_H

#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <ctime>
#include <vector>

template<typename T>
void serialize_value(std::vector<uint8_t>& data, const T& value) {
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(T));
}

template<typename T>
static void serialize_data(std::vector<uint8_t>& data, const T& value, size_t length) {
  const uint8_t* p_value = reinterpret_cast<const uint8_t*>(value.data());
  data.insert(data.end(), p_value, p_value + length);
}

template<std::size_t LENGTH>
class UniqueIDGenerator {
private:
  std::size_t count = 0;
  std::mt19937 gen;

  UniqueIDGenerator() : gen(std::random_device()()) {}

public:
  std::string operator()() {
    std::uniform_int_distribution<> dis('A', 'Z');

    std::stringstream ss;
    ss << sts::setw(3) << std::setfill('0') << ++count;

    while (ss.str().size() < LENGTH)
      ss << static_cast<char>(dis(gen));

    return ss.str();
  }
};

#endif //SPACE_UTILS_H
