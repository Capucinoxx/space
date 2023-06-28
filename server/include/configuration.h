#ifndef SPACE_CONFIG_H
#define SPACE_CONFIG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> Config;

std::string trim(const std::string& str) {
  std::string trimmed = str;
  std::size_t start = trimmed.find_first_not_of(" \t");
  std::size_t end = trimmed.find_last_not_of(" \t");
  if (start != std::string::npos && end != std::string::npos)
    return trimmed.substr(start, end - start + 1);

  return trimmed;
}

Config load_config(const std::string& filename) {
  Config config;

  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open config file: " << filename << std::endl;
    return config;
  }

  std::string line;

  while(std::getline(file, line)) {
    line = trim(line);

    if (line.empty() || line[0] == '#')
      continue;

    std::size_t pos = line.find('=');
    if (pos != std::string::npos) {
      std::string key = trim(line.substr(0, pos));
      std::string value = trim(line.substr(pos + 1));

      config[key] = value;
    }
  }

  return config;
}

#endif //SPACE_CONFIG_H