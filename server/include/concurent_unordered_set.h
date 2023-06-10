#ifndef SPACE_CONCURENT_UNORDERED_SET_H
#define SPACE_CONCURENT_UNORDERED_SET_H

#include <unordered_set>
#include <mutex>

template <typename T, class Hash = std::hash<T>>
class ConcurrentUnorderedSet
{
private:
  std::unordered_set<T, Hash> set{ };
  mutable std::mutex mu;

public:
  ConcurrentUnorderedSet() = default;

  bool insert(const T &value) {
    std::lock_guard<std::mutex> lock(mu);
    return set.insert(value).second;
  }

  void reserve(size_t size) {
    std::lock_guard<std::mutex> lock(mu);
    set.reserve(size);
  }

  bool erase(const T &value) {
    std::lock_guard<std::mutex> lock(mu);
    return set.erase(value);
  }

  bool contains(const T &value) const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return set.find(value) != set.end();
  }

  void clear() noexcept {
    std::lock_guard<std::mutex> lock(mu);
    set.clear();
  }

  size_t size() const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return set.size();
  }

  std::unordered_set<T>::iterator find(const T &value) {
    std::lock_guard<std::mutex> lock(mu);
    return set.find(value);
  }

  std::unordered_set<T>::iterator begin() {
    std::lock_guard<std::mutex> lock(mu);
    return set.begin();
  }

  std::unordered_set<T>::iterator end() {
    std::lock_guard<std::mutex> lock(mu);
    return set.end();
  }
};

#endif // SPACE_CONCURENT_UNORDERED_SET_H