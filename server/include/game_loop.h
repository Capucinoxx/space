#ifndef SPACE_GAME_LOOP_H
#define SPACE_GAME_LOOP_H

#include <thread>
#include <atomic>
#include <type_traits>
#include <utility>

template<typename T, std::size_t TICK, std::size_t ROWS, std::size_t COLS>
class GameLoop {
  static_assert(std::is_same<T, std::pair<typename T::first_type, typename T::second_type>>::value, "T must be a pair");
  static_assert(std::is_same<typename T::first_type, std::shared_ptr<Player<ROWS, COLS>>>::value,
                  "T::first_type must be std::shared_ptr<Player>");

private:
  std::thread th;
  std::atomic<bool> running{ false };
  
  std::shared_ptr<GameState<T, ROWS, COLS>> state;

public:
  GameLoop(std::shared_ptr<GameState<T, ROWS, COLS>> state) 
    : state(std::move(state)) { }

  template<typename Callback>
  void start(Callback* object, void (Callback::*callback)(const std::string&, const std::vector<uint8_t>&), const std::string& channel) { 
    if (is_running())
      return;

    running.store(true);

    th = std::thread([object, callback, channel, this]() {
      while (is_running()) {
        auto data = state->serialize();
        (object->*callback)(channel, data);

        std::this_thread::sleep_for(std::chrono::milliseconds(TICK));

        tick();
      }
    });

  }

  void stop() noexcept {
    if (is_running())
      stop_loop();
  }
  
  bool is_running() const noexcept { return running.load(); }

  void stop_loop() {
    running.store(false);
    if (th.joinable())
      th.join();
  }

private:
  void tick() {
    state->tick();
  }
};

#endif // SPACE_GAME_LOOP_H