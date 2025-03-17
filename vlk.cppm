#pragma leco add_impl vlk_bits
export module vlk;
export import :common;
import dotz;
import hai;
import voo;

namespace vlk {
  export constexpr auto id_to_uv(unsigned id) {
    return dotz::vec2 { id % 16, id / 16 };
  }

  export void map_atlas(hai::fn<void, unsigned *> f);
  export void map_instances(hai::fn<inst *, inst *> f);
  export void map_picks(hai::fn<pickable *, pickable *> f);
  export unsigned current_pick();

  export hai::fn<void> on_init {};
  export hai::fn<void> after_present {};
}
