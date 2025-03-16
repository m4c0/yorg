#pragma leco add_shader "vlk-spr.frag"
#pragma leco add_shader "vlk-spr.vert"
module vlk:spr;
import :common;
import :internal;
import dotz;
import hai;
import voo;

namespace vlk {
  struct upc { float aspect; };

  class spr {
    static constexpr const auto max_inst_count = 1024;

    static auto att() {
      return vee::attachment_description {
        .format = vlk::dq->find_best_surface_image_format(),
        .load_op = vee::attachment_load_op_load,
        .store_op = vee::attachment_store_op_store,
        .initial_layout = vee::image_layout_color_attachment_optimal,
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
        .dependencies {{ vee::create_colour_dependency() }},
      });
    }

    voo::single_dset m_ds {
      vee::dsl_fragment_sampler(),
      vee::combined_image_sampler(),
    };

    voo::host_buffer m_inst {
      dq->physical_device(),
      vee::create_buffer(
        max_inst_count * sizeof(inst),
        vee::buffer_usage::vertex_buffer
      )
    };
    voo::one_quad m_quad { dq->physical_device() };

    vee::render_pass m_rp { rnd_pass() };
    vee::pipeline_layout m_pl { vee::create_pipeline_layout(
      { m_ds.descriptor_set_layout() },
      { vee::vert_frag_push_constant_range<upc>() }
    ) };
    vee::gr_pipeline m_ppl { vee::create_graphics_pipeline({
      .pipeline_layout = *m_pl,
      .render_pass = *m_rp,
      .depth_test = false,
      .blends { vee::colour_blend_classic() },
      .shaders {
        voo::shader("vlk-spr.vert.spv").pipeline_vert_stage(),
        voo::shader("vlk-spr.frag.spv").pipeline_frag_stage(),
      },
      .bindings {
        m_quad.vertex_input_bind(),
        vee::vertex_input_bind_per_instance(sizeof(inst)),
      },
      .attributes {
        m_quad.vertex_attribute(0),
        vee::vertex_attribute_vec2(1, traits::offset_of(&inst::pos)),
        vee::vertex_attribute_vec2(1, traits::offset_of(&inst::uv)),
      },
    }) };

    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    voo::h2l_image m_img { dq->physical_device(), 16, 16, VK_FORMAT_R8G8B8A8_SRGB };

    hai::array<vee::framebuffer> m_fbs = sw->create_framebuffers(*m_rp);

    bool m_dirty_atlas {};
    unsigned m_count {};

  public:
    spr() {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_img.iv(), *m_smp);
    }

    void map_atlas(auto && fn) {
      voo::mapmem mm { m_img.host_memory() };
      fn(static_cast<unsigned *>(*mm));
      m_dirty_atlas = true;
    }
    auto map_instances(auto && fn) {
      voo::mapmem mm { m_inst.memory() };
      auto s = static_cast<inst *>(*mm);
      auto e = fn(s);
      m_count = e - s;
    }

    void cmd_render_pass(vee::command_buffer cb) {
      if (m_dirty_atlas) {
        m_img.setup_copy(cb);
        m_dirty_atlas = false;
      }

      upc pc { sw->aspect() };
      auto scb = voo::cmd_render_pass({
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[sw->index()],
        .extent = sw->extent(),
      });
      vee::cmd_set_viewport(*scb, sw->extent());
      vee::cmd_set_scissor(*scb, sw->extent());
      vee::cmd_push_vert_frag_constants(*scb, *m_pl, &pc);
      vee::cmd_bind_descriptor_set(*scb, *m_pl, 0, m_ds.descriptor_set());
      vee::cmd_bind_gr_pipeline(*scb, *m_ppl);
      vee::cmd_bind_vertex_buffers(*scb, 1, m_inst.buffer());
      m_quad.run(*scb, 0, m_count);
    }
  };
}
