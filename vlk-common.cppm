export module vlk:common;
import dotz;

namespace vlk {
  export struct inst {
    dotz::vec2 pos;
    dotz::vec2 uv;
  };
  static_assert(sizeof(inst) % 4 == 0);
}
