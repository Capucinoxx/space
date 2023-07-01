#ifndef SPACE_TESTS_H
#define SPACE_TESTS_H

#include <iostream>
#include <atomic>
#include <optional>
#include <chrono>

class TestManager {
public:
  TestManager(const TestManager&) = delete;
  TestManager& operator=(const TestManager&) = delete;

private:
  static TestManager singleton;

  std::atomic<std::size_t> passed = 0;
  std::atomic<std::size_t> failed = 0;

public:
  TestManager() = default;
  ~TestManager() {
    std::cout << std::endl;
    std::cout << "=== TEST RESULTS ===" << std::endl;
    std::cout << "Tests passed: " << passed << std::endl;
    std::cout << "Tests failed: " << failed << std::endl;
  };

  static TestManager& get() noexcept { return singleton; }

  void success() noexcept { passed++; }
  void failure() noexcept { failed++; }
};

TestManager TestManager::singleton;

namespace assert {
  template<class F, class ... Args>
  auto test(F f, Args &&... args) {
    auto pre = std::chrono::high_resolution_clock::now();
    f(std::forward<Args>(args)...);
    auto post = std::chrono::high_resolution_clock::now();
    return post - pre;
  }

  template<class F, class ... Args>
  void it(const std::string& name, F f, Args &&... args) {
    std::cout << "Test: " << name << std::endl;
    auto dt = test(f, std::forward<Args>(args)...);
    std::cout << "\tTook: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(dt).count() << " us."
              << std::endl;
  }

  template<typename T, typename U>
  void equal(const T& lhs, const U& rhs) {
    if (lhs == rhs) {
      TestManager::get().success();
    } else {
      TestManager::get().failure();
      std::cerr << "\n\tAssertion failed: " << lhs << " == " << rhs << std::endl;
    }
  }

  template<typename T, typename U>
  void not_equal(const T& lhs, const U& rhs) {
    if (lhs != rhs) {
      TestManager::get().success();
    } else {
      TestManager::get().failure();
      std::cerr << "\n\tAssertion failed: " << lhs << " != " << rhs << std::endl;
    }
  }

  void is_true(const bool& condition) {
    if (condition) {
      TestManager::get().success();
    } else {
      TestManager::get().failure();
      std::cerr << "\n\tAssertion failed: " << condition << " == true" << std::endl;
    }
  }

  void is_false(const bool& condition) {
    if (!condition) {
      TestManager::get().success();
    } else {
      TestManager::get().failure();
      std::cerr << "\n\tAssertion failed: " << condition << " == false" << std::endl;
    }
  }
};

#endif //SPACE_TESTS_H