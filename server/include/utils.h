#ifndef SPACE_UTILS_H
#define SPACE_UTILS_H

#include <vector>

template<typename T>
void serialize_value(std::vector<uint8_t>& data, const T& value) {
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(T));
}

template<typename T>
static void serialize_data(std::vector<uint8_t>& data, const T& value, size_t length) {
  const uint8_t* pValue = reinterpret_cast<const uint8_t*>(value.data());
  data.insert(data.end(), pValue, pValue + length);
}

#endif //SPACE_UTILS_H
