#pragma leco add_impl vlk_bits
export module vlk;
import hai;
import voo;

namespace vlk {
  export class bits {
  public:
    static hai::uptr<bits> create(voo::device_and_queue * dq);
  };
}
