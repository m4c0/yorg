export module soldiers;
import vlk;

namespace soldiers {
  export auto load_sprites(auto * i) {
    *i++ = {
      .pos { 3, 1 },
      .uv = vlk::id_to_uv('S'),
    };
    *i++ = {
      .pos { 8, 12 },
      .uv = vlk::id_to_uv('S'),
    };
    *i++ = {
      .pos { 14, 14 },
      .uv = vlk::id_to_uv('S'),
    };
    return i;
  }
}
