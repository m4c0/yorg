export module atlas;
import dotz;
import traits;
import vee;
import voo;

namespace atlas {
  export constexpr auto id_to_uv(unsigned id) {
    return dotz::vec2 { id % 16, id / 16 };
  }

  export class t {
    voo::h2l_image m_img;
    voo::single_cb m_cb;

  public:
    t(vee::physical_device pd, unsigned qf)
      : m_img { pd, 16, 16, VK_FORMAT_R8G8B8A8_SRGB }
      , m_cb { qf }
    {}

    auto mapmem(voo::queue * q, traits::is_callable<unsigned *> auto fn) {
      {
        voo::mapmem mm { m_img.host_memory() };
        fn(static_cast<unsigned *>(*mm));
      }
      {
        voo::cmd_buf_one_time_submit ots { m_cb.cb() };
        m_img.setup_copy(m_cb.cb());
      }
      q->queue_submit({ .command_buffer = m_cb.cb() });
    }

    constexpr auto image_view() const { return m_img.iv(); }
  };
};
