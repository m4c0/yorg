module vlk:clear;
import :internal;
import hai;
import voo;

static auto att() {
  return vee::attachment_description {
    .format = vlk::dq->find_best_surface_image_format(),
    .load_op = vee::attachment_load_op_clear,
    .store_op = vee::attachment_store_op_store,
    .initial_layout = vee::image_layout_undefined,
    .final_layout = vee::image_layout_color_attachment_optimal,
  };
}
static auto rnd_pass() {
  return vee::create_render_pass(vee::create_render_pass_params {
    .attachments {{ vee::create_colour_attachment(att()) }},
    .subpasses {{
      vee::create_subpass({
        .colours {{
          vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal),
        }},
      }),
    }},
    .dependencies {{ vee::create_colour_dependency() }}
  });
}

namespace vlk::impl {
  class clear {
    vee::render_pass m_rp;
    hai::array<vee::framebuffer> m_fbs;

  public:
    clear(voo::swapchain * sw)
      : m_rp { rnd_pass() }
      , m_fbs { sw->create_framebuffers(*m_rp) }
    {}

    void cmd_render_pass(vee::command_buffer cb, voo::swapchain * sw) {
      voo::cmd_render_pass({
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[sw->index()],
        .extent = sw->extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
      });
    }
  };
}
