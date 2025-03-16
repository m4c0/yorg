#pragma leco add_shader "cursor.frag"
#pragma leco add_shader "cursor.vert"
export module cursor;
import casein;
import dotz;
import hai;
import render;
import vee;
import voo;

namespace cursor {
  struct upc {
    dotz::vec2 anchor;
    dotz::vec2 window_size;
  };

  export class system {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      { vee::vert_frag_push_constant_range<upc>() });
    vee::render_pass m_rp;
    voo::one_quad_render m_oqr;
    hai::array<vee::framebuffer> m_fbs;

  public:
    explicit system(render::system * rnd)
      : m_rp { 
        vee::create_render_pass({
          .attachments {{
            vee::create_colour_attachment({
              .format = rnd->dq->find_best_surface_image_format(),
              .load_op = vee::attachment_load_op_load,
              .store_op = vee::attachment_store_op_store,
              .initial_layout = vee::image_layout_color_attachment_optimal,
              .final_layout = vee::image_layout_present_src_khr,
            }),
          }},
          .subpasses {{
            vee::create_subpass({
              .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
            }),
          }},
          .dependencies {{
            vee::create_colour_dependency(),
          }},
        })
      }
      , m_oqr { "cursor", rnd->dq->physical_device(), *m_rp, *m_pl }
      , m_fbs { rnd->sw->create_framebuffers(*m_rp) }
    {}

    void cmd_render_pass(render::system * rnd) {
      upc pc {
        .anchor = casein::mouse_pos,
        .window_size = casein::window_size,
      };
      auto scb = voo::cmd_render_pass(vee::render_pass_begin {
        .command_buffer = rnd->cb.cb(),
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[rnd->sw->index()],
        .extent = rnd->sw->extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
        .clear_depth = false,
      });
      vee::cmd_push_vert_frag_constants(rnd->cb.cb(), *m_pl, &pc);
      m_oqr.run(rnd->cb.cb(), rnd->sw->extent());
    }
  };
}

