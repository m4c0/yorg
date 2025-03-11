export module render;
import hai;
import voo;

namespace render {
  export constexpr const auto select_format = VK_FORMAT_R32_UINT;

  export class offscreen {
    hai::array<voo::offscreen::colour_buffer> m_sel;
    const voo::swapchain * m_sw;

  public:
    explicit offscreen(voo::device_and_queue & dq, const voo::swapchain & sw)
      : m_sel { sw.count() }
      , m_sw { &sw }
    {
      for (auto i = 0; i < m_sel.size(); i++) {
        m_sel[i] = {
          dq.physical_device(), sw.extent(), select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };
      }
    }
    [[nodiscard]] constexpr auto aspect() const { return m_sw->aspect(); }
    [[nodiscard]] constexpr auto count() const { return m_sel.size(); }
    [[nodiscard]] constexpr auto extent() const { return m_sw->extent(); }
    [[nodiscard]] constexpr auto image() const { return m_sel[m_sw->index()].image(); }
    [[nodiscard]] constexpr auto image_view(unsigned idx) const { return m_sel[idx].image_view(); }
    [[nodiscard]] constexpr auto index() const { return m_sw->index(); }
  };

  export struct system {
    voo::device_and_queue * dq {};
    voo::swapchain sw { *dq };
    voo::single_cb cb { dq->queue_family() };
    offscreen ofs { *dq, sw };
  };
}
