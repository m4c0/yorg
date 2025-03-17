export module state;
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
  export void foreach(auto fn) {
    fn(3, 1);
    fn(8, 12);
    fn(14, 14);
  }
}

namespace state::enemies {
  export void foreach(auto fn) {
    fn(7, 4);
    fn(8, 5);
  }
}
