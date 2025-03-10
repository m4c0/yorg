export module render;
import pick;
import voo;

namespace render {
  export struct system {
    voo::device_and_queue * dq {};
    voo::swapchain sw { *dq };
    pick::offscreen ofs { *dq, sw };
  };
}
