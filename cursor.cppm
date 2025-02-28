#pragma leco add_shader "cursor.frag"
#pragma leco add_shader "cursor.vert"
export module cursor;
import hai;
import vee;
import voo;

namespace cursor {
  export class t {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout();
    vee::render_pass m_rp;
    voo::one_quad_render m_oqr;
    hai::array<vee::framebuffer> m_fbs;

  public:
    explicit t(const voo::device_and_queue * dq, const voo::swapchain & sw)
      : m_rp { vee::create_depthless_render_pass(dq->physical_device(), dq->surface()),}
      , m_oqr { "cursor", dq->physical_device(), *m_rp, *m_pl }
      , m_fbs { sw.create_framebuffers(*m_rp) }
    {}

    void run(vee::command_buffer cb, voo::swapchain & sw) {
      auto scb = voo::cmd_render_pass(vee::render_pass_begin {
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[sw.index()],
        .extent = sw.extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
        .clear_depth = false,
      });
      m_oqr.run(cb, sw.extent(), [] {
      });
    }
  };
}

