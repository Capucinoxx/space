#ifndef SPACE_CONCURRENT_UNORDERED_MAP_H
#define SPACE_CONCURRENT_UNORDERED_MAP_H

#include <unordered_map>
#include <mutex>
#include <algorithm>

template <typename K, typename V, class Hash = std::hash<K>>
class c_unordered_map {
private:
  std::unordered_map<K, V, Hash> map{ };
  mutable std::mutex mu;

public:
  c_unordered_map() = default;

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

  std::size_t size() const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return map.size();
  }

  template<typename F>
  void for_each(F&& f) {
    std::lock_guard<std::mutex> lock(mu);
    std::for_each(map.begin(), map.end(), f);
  }

  V operator[](const K &key) {
    std::lock_guard<std::mutex> lock(mu);
    return map[key];
  }

  template<typename F>
  void do_at(const K& key, F&& f) {
    std::lock_guard<std::mutex> lock(mu);
    f(map[key]);
  }
};

#endif //SPACE_CONCURRENT_UNORDERED_MAP_H