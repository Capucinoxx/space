#include <vector>
#include <unordered_map>


#include "utils.h"

int main() {
  std::unordered_map<std::string, Scenario> scenarios;

  scenarios.insert({ "dummy test", {
    Bots{ Bot{ 1, position{ 10, 10 } } },
    Ticks{
      actions{ { 1, UP } },
      actions{ { 1, UP } },
      actions{ { 1, UP } },
    },
    Expectations{ 
      expectation{ 1, position{ 10, 13 } } 
    },
  }
  });

  for (auto& [name, scenario] : scenarios)
    run_scenario(name, scenario);
  
  return 0;
}