#ifndef SPACE_CONCURRENT_UNORDERED_MAP_H
#define SPACE_CONCURRENT_UNORDERED_MAP_H

#include <unordered_map>
#include <mutex>

template <typename K, typename V, class Hash = std::hash<K>>
class ConcurrentUnorderedMap {
private:
  std::unordered_map<K, V, Hash> map{ };
  mutable std::mutex mu;

public:
  ConcurrentUnorderedMap() = default;

  bool insert(const K &key, const V &value) {
    std::lock_guard<std::mutex> lock(mu);
    return map.insert(std::make_pair(key, value)).second;
  }

  bool erase(const K &key) {
    std::lock_guard<std::mutex> lock(mu);
    return map.erase(key);
  }

  std::unordered_map<K, V>::iterator find(const K &key) {
    std::lock_guard<std::mutex> lock(mu);
    return map.find(key);
  }

  bool contains(const K &key) const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return map.find(key) != map.end();
  }

  std::unordered_map<K, V>::iterator begin() {
    std::lock_guard<std::mutex> lock(mu);
    return map.begin();
  }

  std::unordered_map<K, V>::iterator end() {
    std::lock_guard<std::mutex> lock(mu);
    return map.end();
  }
};

#endif //SPACE_CONCURRENT_UNORDERED_MAP_H