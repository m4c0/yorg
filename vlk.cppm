#pragma leco add_impl vlk_bits
export module vlk;
import hai;
import voo;

namespace vlk {
  export class bits {
  public:
    virtual ~bits() {}

    virtual void present() = 0;

    static hai::uptr<bits> create(voo::device_and_queue * dq);
  };
}
