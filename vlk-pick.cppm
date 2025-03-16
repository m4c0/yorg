#pragma leco add_shader "vlk-pick.frag"
#pragma leco add_shader "vlk-pick.vert"
module vlk:pick;
import :common;
import :internal;
import casein;
import hai;
import voo;

namespace vlk {
  struct upc { float aspect; };

  constexpr const auto select_format = VK_FORMAT_R32_UINT;

  class pick {
    static constexpr const auto max_inst_count = 1024;

    voo::host_buffer m_inst {
      dq->physical_device(),
      vee::create_buffer(
        max_inst_count * sizeof(pickable),
        vee::buffer_usage::vertex_buffer
      )
    };
    voo::one_quad m_quad { dq->physical_device() };

    vee::render_pass m_rp { vee::create_render_pass(vee::create_render_pass_params {
      .attachments {{
        vee::create_colour_attachment(select_format, vee::image_layout_transfer_src_optimal),
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
    }) };
    vee::pipeline_layout m_pl {
      vee::create_pipeline_layout({ vee::vert_frag_push_constant_range<upc>() })
    };
    vee::gr_pipeline m_ppl { vee::create_graphics_pipeline({
      .pipeline_layout = *m_pl,
      .render_pass = *m_rp,
      .depth_test = false,
      .blends { vee::colour_blend_none() },
      .shaders {
        voo::shader("vlk-pick.vert.spv").pipeline_vert_stage(),
        voo::shader("vlk-pick.frag.spv").pipeline_frag_stage(),
      },
      .bindings {
        m_quad.vertex_input_bind(),
        vee::vertex_input_bind_per_instance(sizeof(inst)),
      },
      .attributes {
        m_quad.vertex_attribute(0),
        vee::vertex_attribute_vec2(1, traits::offset_of(&pickable::pos)),
        vee::vertex_attribute_uint(1, traits::offset_of(&pickable::id)),
      },
    }) };

    voo::host_buffer m_pick { dq->physical_device(), vee::create_transfer_dst_buffer(sizeof(unsigned)) };

    hai::array<voo::offscreen::colour_buffer> m_sel { sw->count() };
    hai::array<vee::framebuffer> m_fbs { sw->count() };

    unsigned m_count {};

  public:
    pick() : m_fbs {} {
      for (auto i = 0; i < sw->count(); i++) {
        m_sel[i] = {
          dq->physical_device(), sw->extent(), select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };

        m_fbs[i] = vee::create_framebuffer({
          .render_pass = *m_rp,
          .attachments {{ m_sel[i].image_view() }},
          .extent = sw->extent(),
        });
      }
    }

    auto map(auto && fn) {
      voo::mapmem mm { m_inst.memory() };
      auto s = static_cast<pickable *>(*mm);
      auto e = fn(s);
      m_count = e - s;
    }

    void cmd_render_pass(vee::command_buffer cb) {
      auto mouse = casein::mouse_pos;
      if (mouse.x < 0 || mouse.x >= casein::window_size.x ||
          mouse.y < 0 || mouse.y >= casein::window_size.y)
        return;

      int mx = mouse.x * casein::screen_scale_factor;
      int my = mouse.y * casein::screen_scale_factor;

      upc pc { sw->aspect() };
      auto scb = voo::cmd_render_pass({
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[sw->index()],
        .extent = sw->extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
      });
      vee::cmd_set_viewport(*scb, sw->extent());
      vee::cmd_set_scissor(*scb, vee::rect { { mx, my }, { 1, 1 } });
      vee::cmd_push_vert_frag_constants(*scb, *m_pl, &pc);
      vee::cmd_bind_gr_pipeline(*scb, *m_ppl);
      vee::cmd_bind_vertex_buffers(*scb, 1, m_inst.buffer());
      m_quad.run(*scb, 0, m_count);

      auto iv = m_sel[sw->index()].image();
      vee::cmd_copy_image_to_buffer(cb, { mx, my }, { 1, 1 }, iv, m_pick.buffer());
    }

    int current() {
      voo::mapmem mm { m_pick.memory() };
      return *static_cast<unsigned *>(*mm);
    }
  };
}
