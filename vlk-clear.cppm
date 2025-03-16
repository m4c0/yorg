module vlk:clear;
import :internal;
import hai;
import voo;

namespace vlk::impl {
  class clear {
    static auto att() {
      return vee::attachment_description {
        .format = vlk::dq->find_best_surface_image_format(),
        .load_op = vee::attachment_load_op_clear,
        .store_op = vee::attachment_store_op_store,
        .initial_layout = vee::image_layout_undefined,
        .final_layout = vee::image_layout_color_attachment_optimal,
      };
    }
    static vee::render_pass rnd_pass() {
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

    vee::render_pass m_rp { rnd_pass() };
    hai::array<vee::framebuffer> m_fbs { sw->create_framebuffers(*m_rp) };

  public:
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
