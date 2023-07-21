#ifndef SPACE_CONCURRENT_UNORDERED_SET_H
#define SPACE_CONCURRENT_UNORDERED_SET_H

#include <unordered_set>
#include <mutex>
#include <algorithm>

template <typename T, class Hash = std::hash<T>>
class c_unordered_set
{
private:
  std::unordered_set<T, Hash> set{ };
  mutable std::mutex mu;

public:
  c_unordered_set() = default;

  bool insert(const T &value) {
    std::lock_guard<std::mutex> lock(mu);
    return set.insert(value).second;
  }

  std::unordered_set<T, Hash>& get() {
    std::lock_guard<std::mutex> lock(mu);
    return set;
  }

  template<typename C>
  void insert(C begin, C end) {
    std::lock_guard<std::mutex> lock(mu);
    set.insert(begin, end);
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

  template<typename F>
  void for_each(F&& f) {
    std::lock_guard<std::mutex> lock(mu);
    std::for_each(set.begin(), set.end(), f);
  }
};

#endif // SPACE_CONCURRENT_UNORDERED_SET_H