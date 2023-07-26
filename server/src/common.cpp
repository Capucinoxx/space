#include "common.h"

std::optional<std::tuple<double, double, double>> parse_color(const std::string& color) {
  std::istringstream iss(color);
  std::vector<double> hsl;
  double value;

  while (iss >> value) {
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

std::string_view retrieve_channel(const std::string& str) {
  std::size_t start = str.find_first_of('/');
  if (start == std::string::npos)
    return std::string_view();

  std::size_t end = str.find_first_of('/', start + 1);
  if (end == std::string::npos)
    return std::string_view();

  return std::string_view(str.data() + start + 1, end - start - 1);
}

std::string retrieve_field(const std::string& body, const std::string& field) {
    std::size_t pos = body.find(field);
    if (pos == std::string::npos)
      return "";

    std::size_t start = pos + field.size() + 1;
    std::size_t end = body.find('&', start);
    if (end == std::string::npos)
      end = body.size();

    return body.substr(start, end - start);
  };

uint64_t parse_string_to_epoch(const std::string& str) {
  std::tm tm = {};
  std::istringstream iss(str);
  iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  if (iss.fail() || std::mktime(&tm) == -1)
    return 0;

  std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

  return static_cast<uint64_t>(timestamp.time_since_epoch().count());
}