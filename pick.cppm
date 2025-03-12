#pragma leco add_shader "pick.frag"
#pragma leco add_shader "pick.vert"
export module pick;
import dotz;
import hai;
import render;
import traits;
import voo;

namespace pick {
  export struct inst {
    dotz::vec2 pos;
    unsigned id;
    float pad;
  };
  static_assert(sizeof(inst) % 16 == 0);

  struct upc { float aspect; };

  export class system {
    static constexpr const auto max_inst_count = 1024;

    voo::host_buffer m_inst;
    voo::one_quad m_quad;

    vee::render_pass m_rp;
    vee::pipeline_layout m_pl;
    vee::gr_pipeline m_ppl;

    voo::host_buffer m_pick;

    hai::array<vee::framebuffer> m_fbs;

    unsigned m_count {};

    void cmd_render_pass(render::system * rnd, int mx, int my) {
      upc pc { rnd->ofs.aspect() };
      auto scb = voo::cmd_render_pass({
        .command_buffer = rnd->cb.cb(),
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[rnd->ofs.index()],
        .extent = rnd->ofs.extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
      });
      vee::cmd_set_viewport(*scb, rnd->ofs.extent());
      vee::cmd_set_scissor(*scb, vee::rect { { mx, my }, { 1, 1 } });
      vee::cmd_push_vert_frag_constants(*scb, *m_pl, &pc);
      vee::cmd_bind_gr_pipeline(*scb, *m_ppl);
      vee::cmd_bind_vertex_buffers(*scb, 1, m_inst.buffer());
      m_quad.run(*scb, 0, m_count);
    }
    void cmd_copy_image_to_buffer(render::system * rnd, int mx, int my) {
      vee::cmd_copy_image_to_buffer(rnd->cb.cb(), { mx, my }, { 1, 1 }, rnd->ofs.image(), m_pick.buffer());
    }

  public:
    system(const voo::device_and_queue * dq, const render::offscreen & ofs)
      : m_inst {
        dq->physical_device(),
        vee::create_buffer(
          max_inst_count * sizeof(inst),
          vee::buffer_usage::vertex_buffer
        )
      }
      , m_quad { dq->physical_device() }
      , m_rp { vee::create_render_pass(vee::create_render_pass_params {
        .attachments {{
          vee::create_colour_attachment(render::select_format, vee::image_layout_transfer_src_optimal),
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
      , m_pl { vee::create_pipeline_layout({ vee::vert_frag_push_constant_range<upc>() }) }
      , m_ppl { vee::create_graphics_pipeline({
        .pipeline_layout = *m_pl,
        .render_pass = *m_rp,
        .depth_test = false,
        .blends { vee::colour_blend_none() },
        .shaders {
          voo::shader("pick.vert.spv").pipeline_vert_stage(),
          voo::shader("pick.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          m_quad.vertex_input_bind(),
          vee::vertex_input_bind_per_instance(sizeof(inst)),
        },
        .attributes {
          m_quad.vertex_attribute(0),
          vee::vertex_attribute_vec2(1, traits::offset_of(&inst::pos)),
          vee::vertex_attribute_uint(1, traits::offset_of(&inst::id)),
        },
      }) }
      , m_pick { dq->physical_device(), vee::create_transfer_dst_buffer(sizeof(unsigned)) }
      , m_fbs { ofs.count() }
    {
      for (auto i = 0; i < m_fbs.size(); i++) {
        m_fbs[i] = vee::create_framebuffer({
          .render_pass = *m_rp,
          .attachments {{ ofs.image_view(i) }},
          .extent = ofs.extent(),
        });
      }
    }

    auto map() { return voo::memiter<inst> { m_inst.memory(), &m_count }; }

    void run(render::system * rnd, int mx, int my) {
      cmd_render_pass(rnd, mx, my);
      cmd_copy_image_to_buffer(rnd, mx, my);
    }

    int pick() {
      voo::mapmem mm { m_pick.memory() };
      return *static_cast<unsigned *>(*mm);
    }
  };
}
