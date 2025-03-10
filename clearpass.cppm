export module clearpass;
import hai;
import render;
import voo;

namespace clearpass {
  static auto att(voo::device_and_queue * dq) {
    return vee::attachment_description {
      .format = dq->find_best_surface_image_format(),
      .load_op = vee::attachment_load_op_clear,
      .store_op = vee::attachment_store_op_store,
      .initial_layout = vee::image_layout_undefined,
      .final_layout = vee::image_layout_color_attachment_optimal,
    };
  }

  export class system {
    vee::render_pass m_rp;
    hai::array<vee::framebuffer> m_fbs;
  public:
    system(render::system * rnd)
      : m_rp { vee::create_render_pass(vee::create_render_pass_params {
        .attachments {{
          vee::create_colour_attachment(att(rnd->dq)),
        }},
        .subpasses {{
          vee::create_subpass({
            .colours {{
              vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal),
            }},
          }),
        }},
        .dependencies {{
          vee::create_colour_dependency(),
        }},
      }) }
      , m_fbs { rnd->sw.count() }
    {
      for (auto i = 0; i < m_fbs.size(); i++) {
        m_fbs[i] = vee::create_framebuffer({
          .render_pass = *m_rp,
          .attachments {{ rnd->sw.image_view(i) }},
          .extent = rnd->sw.extent(),
        });
      }
    }

    void cmd_render_pass(render::system * rnd) {
      voo::cmd_render_pass({
        .command_buffer = rnd->cb.cb(),
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[rnd->sw.index()],
        .extent = rnd->sw.extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
      });
    }
  };
}
