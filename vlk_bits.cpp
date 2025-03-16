module vlk;
import :clear;
import :spr;

namespace vlk::impl {
  class bits : public vlk::bits {
    voo::swapchain m_sw { *dq };
    voo::single_cb m_cb { dq->queue_family() };
    voo::frame_sync_stuff m_sync {};
    hai::array<voo::offscreen::colour_buffer> m_sel { m_sw.count() };

    clear m_clr {};
    spr m_spr {};

  public:
    static constexpr const auto select_format = VK_FORMAT_R32_UINT;

    explicit bits() {
      vlk::sw = &m_sw;

      for (auto i = 0; i < m_sel.size(); i++) {
        m_sel[i] = {
          dq->physical_device(), m_sw.extent(), select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };
      }
    }

    void present(hai::fn<void> fn) override {
      voo::present_guard pg { dq->queue(), &m_sw, &m_sync };
      {
        voo::cmd_buf_one_time_submit pcb { m_cb.cb() };
        m_clr.cmd_render_pass(*pcb);
        m_spr.cmd_render_pass(*pcb);
        fn();
      }
      m_sync.queue_submit(dq->queue(), m_cb.cb());
    }
  };
}
namespace vlk {
  hai::uptr<bits> bits::create(voo::device_and_queue * dq) {
    vlk::dq = dq;
    return hai::uptr<bits> { new impl::bits {} };
  }
}

