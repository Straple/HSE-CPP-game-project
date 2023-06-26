#include "utils.hpp"

std::mt19937 &get_random_engine() {
  static std::mt19937 rnd(13);
  return rnd;
}

std::string to_string(efloat val) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(4) << val;
  std::string s = oss.str();
  if (s == "nan(ind)" || s == "-nan(ind)") {
    s = "nan";
  }
  return s;
}

bool randomness(unsigned int percent) {
  ASSERT(percent <= 100, "wrong percent");

  std::uniform_int_distribution<unsigned int> rnd_range(0, 100);

  return rnd_range(get_random_engine()) <= percent;
}