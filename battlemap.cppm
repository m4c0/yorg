export module battlemap;
import jute;
import vlk;

namespace battlemap {
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
