#pragma leco add_shader "spr.frag"
#pragma leco add_shader "spr.vert"
module vlk:spr;
import :internal;
import dotz;
import hai;
import voo;

namespace vlk {
  struct inst {
    dotz::vec2 pos;
    dotz::vec2 uv;
  };
  static_assert(sizeof(inst) % 4 == 0);
  
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
        voo::shader("spr.vert.spv").pipeline_vert_stage(),
        voo::shader("spr.frag.spv").pipeline_frag_stage(),
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

    hai::array<vee::framebuffer> m_fbs = sw->create_framebuffers(*m_rp);

    unsigned m_count {};

  public:
    auto map() { return voo::memiter<inst> { m_inst.memory(), &m_count }; }

    void update_atlas(vee::image_view::type iv, vee::sampler::type smp) {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, iv, smp);
    }

    void cmd_render_pass(vee::command_buffer cb) {
      upc pc { sw->aspect() };
      auto scb = voo::cmd_render_pass({
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = *m_fbs[sw->index()],
        .extent = sw->extent(),
        .clear_colours { vee::clear_colour(0, 0, 0, 0) },
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
