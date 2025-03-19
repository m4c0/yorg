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
  hai::array<dotz::vec2> all {{
    { 3, 1 },
    { 8, 12 },
    { 14, 14 },
  }};

  export void foreach(auto fn) {
    for (auto [x, y] : all) fn(x, y);
  }
}

namespace state::enemies {
  export void foreach(auto fn) {
    fn(7, 4);
    fn(8, 5);
  }
}
