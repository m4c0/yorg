export module state;
import dotz;
import hai;
import jute;

namespace state::battlemap {
  static constexpr const jute::view map {
    "................"
    "................"
    "....XXX........."
    "..XXXXX........."
    "..XXX..........."
    "...XXXX........."
    "....X..........."
    "................"
    "..........X....."
    "..........XXX..."
    "..........XXXXX."
    "............XXX."
    "..............X."
    "................"
    "................"
    "................"
  };
  static_assert(map.size() == 256);

  export void foreach(auto fn) {
    for (auto i = 0U; i < 256; i++)
      fn(i % 16, i / 16, map[i]);
  }
}

namespace state::soldiers {
  hai::varray<dotz::ivec2> all { 32 };

  export void init() {
    all.push_back(dotz::ivec2 { 3, 1 });
    all.push_back(dotz::ivec2 { 8, 12 });
    all.push_back(dotz::ivec2 { 14, 14 });
  }
  export const auto & list() { return all; }
  export auto & at(int x, int y) {
    for (auto & p : all) {
      if (p != dotz::ivec2 { x, y }) continue;
      return p;
    }
    throw "unreachable";
  }
  export bool has(int x, int y) {
    for (auto & p : all) {
      if (p != dotz::ivec2 { x, y }) continue;
      return true;
    }
    return false;
  }
  export void hit(int x, int y) {
    for (auto i = 0; i < all.size(); i++) {
      auto & p = all[i];
      if (p != dotz::ivec2 { x, y }) continue;
      p = all.pop_back();
      return;
    }
  }
}

namespace state::enemies {
  hai::varray<dotz::ivec2> all { 32 };

  export void init() {
    all.push_back(dotz::ivec2 { 7, 4 });
    all.push_back(dotz::ivec2 { 8, 5 });
  }
  export const auto & list() { return all; }
  export bool has(int x, int y) {
    for (auto & p : all) {
      if (p != dotz::ivec2 { x, y }) continue;
      return true;
    }
    return false;
  }
  export void hit(int x, int y) {
    for (auto i = 0; i < all.size(); i++) {
      auto & p = all[i];
      if (p != dotz::ivec2 { x, y }) continue;
      p = all.pop_back();
      return;
    }
  }
}
