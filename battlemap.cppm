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

  export auto load_sprites(auto * inst) {
    for (auto i = 0U; i < 256; i++) {
      *inst++ = {
        .pos { i % 16, i / 16 },
        .uv = vlk::id_to_uv(map[i]),
      };
    }
    return inst;
  }
  export auto load_pickables(auto * inst) {
    for (auto i = 0U; i < 256; i++) {
      *inst++ = {
        .pos { i % 16, i / 16 },
        .id = i,
      };
    }
    return inst;
  }
}
