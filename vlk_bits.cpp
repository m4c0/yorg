module vlk;
import :clear;

namespace vlk::impl {
  class bits : public vlk::bits {
    voo::device_and_queue * m_dq;
    voo::swapchain m_sw { *m_dq };
    voo::single_cb m_cb { m_dq->queue_family() };
    voo::frame_sync_stuff m_sync {};
    hai::array<voo::offscreen::colour_buffer> m_sel { m_sw.count() };

    clear m_clr { m_dq, &m_sw };

  public:
    static constexpr const auto select_format = VK_FORMAT_R32_UINT;

    explicit bits(voo::device_and_queue * dq) : m_dq { dq } {
      for (auto i = 0; i < m_sel.size(); i++) {
        m_sel[i] = {
          m_dq->physical_device(), m_sw.extent(), select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };
      }
    }

    void present(hai::fn<void> fn) override {
      voo::present_guard pg { m_dq->queue(), &m_sw, &m_sync };
      {
        voo::cmd_buf_one_time_submit pcb { m_cb.cb() };
        m_clr.cmd_render_pass(m_cb.cb(), &m_sw);
        fn();
      }
      m_sync.queue_submit(m_dq->queue(), m_cb.cb());
    }
  };
}
namespace vlk {
  hai::uptr<bits> bits::create(voo::device_and_queue * dq) {
    return hai::uptr<bits> { new impl::bits { dq } };
  }
}

