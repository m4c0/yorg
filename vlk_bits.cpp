module vlk;

namespace vlk::impl {
  class bits : public vlk::bits {
    voo::device_and_queue * m_dq;
    voo::swapchain m_sw { *m_dq };
    voo::single_cb m_cb { m_dq->queue_family() };
    hai::array<voo::offscreen::colour_buffer> m_sel { m_sw.count() };

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
  };
}
namespace vlk {
  hai::uptr<bits> bits::create(voo::device_and_queue * dq) {
    return hai::uptr<bits> { new impl::bits { dq } };
  }
}

