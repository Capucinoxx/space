#ifndef SPACE_CONCURRENT_QUEUE_H
#define SPACE_CONCURRENT_QUEUE_H

#include <queue>
#include <mutex>
#include <atomic>

template <typename T>
class c_queue {
private:
  std::queue<T> queue{ };
  mutable std::mutex mu;
  std::atomic<bool> can_add{ true };

public:
  c_queue() = default;

  void push(const T &value) {
    if (!can_add)
      return;

    std::lock_guard<std::mutex> lock(mu);
    queue.push(value);
  }

  template<typename F>
  void for_each(F &&f) {
    can_add = false;
    std::lock_guard<std::mutex> lock(mu);
    while (!queue.empty()) {
      f(queue.front());
      queue.pop();
    }

    can_add = true;
  }
};

#endif //SPACE_CONCURRENT_QUEUE_H