export module enemies;
import vlk;

namespace enemies {
  export auto load_sprites(auto * i) {
    *i++ = {
      .pos { 7, 4 },
      .uv = vlk::id_to_uv('E'),
    };
    *i++ = {
      .pos { 8, 5 },
      .uv = vlk::id_to_uv('E'),
    };
    return i;
  }
}
