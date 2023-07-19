#ifndef SPACE_CONFIG_H
#define SPACE_CONFIG_H

#include <unordered_map>
#include <string>

typedef std::unordered_map<std::string, std::string> Config;

std::string trim(const std::string& str);

Config load_config(const std::string& filename);

#endif //SPACE_CONFIG_H