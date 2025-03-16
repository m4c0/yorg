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

  export class bits {
  public:
    virtual ~bits() {}

    virtual void map_atlas(hai::fn<void, unsigned *> f) = 0;
    virtual void map_instances(hai::fn<inst *, inst *> f) = 0;
    virtual void map_picks(hai::fn<pickable *, pickable *> f) = 0;
    virtual void present() = 0;

    static hai::uptr<bits> create(voo::device_and_queue * dq);
  };
}
