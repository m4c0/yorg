export module spr;
import dotz;
import traits;
import vee;
import voo;

namespace spr {
  export constexpr const auto select_format = VK_FORMAT_R8G8B8A8_UNORM;

  export struct inst {
    dotz::vec2 pos;
    dotz::vec2 uv;
  };

  struct upc { float aspect; };

  export class system {
    static constexpr const auto inst_count = 256;
    static constexpr const auto max_inst_count = inst_count;

    voo::single_dset m_ds {
      vee::dsl_fragment_sampler(),
      vee::combined_image_sampler(),
    };

    voo::host_buffer m_inst;
    voo::one_quad m_quad;

    vee::render_pass m_rp;
    vee::pipeline_layout m_pl;
    vee::gr_pipeline m_ppl;

  public:
    system(vee::physical_device pd, vee::surface::type s)
      : m_inst {
        pd,
        vee::create_buffer(
          max_inst_count * sizeof(inst),
          vee::buffer_usage::vertex_buffer
        )
      }
      , m_quad { pd }
      , m_rp { vee::create_render_pass(vee::create_render_pass_params {
        .attachments {{
          vee::create_colour_attachment(pd, s),
          vee::create_colour_attachment(select_format, vee::image_layout_transfer_src_optimal),
        }},
        .subpasses {{
          vee::create_subpass({
            .colours {{
              vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal),
              vee::create_attachment_ref(1, vee::image_layout_color_attachment_optimal),
            }},
          }),
        }},
        .dependencies {{
          vee::create_colour_dependency(),
        }},
      }) }
      , m_pl { vee::create_pipeline_layout(
        { m_ds.descriptor_set_layout() },
        { vee::vert_frag_push_constant_range<upc>() }
      ) }
      , m_ppl { vee::create_graphics_pipeline({
        .pipeline_layout = *m_pl,
        .render_pass = *m_rp,
        .depth_test = false,
        .blends {
          vee::colour_blend_classic(),
          vee::colour_blend_none(),
        },
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          m_quad.vertex_input_bind(),
          vee::vertex_input_bind_per_instance(sizeof(inst)),
        },
        .attributes {
          m_quad.vertex_attribute(0),
          vee::vertex_attribute_vec2(1, 0),
          vee::vertex_attribute_vec2(1, sizeof(dotz::vec2)),
        },
      }) }
    {}

    auto render_pass() const { return *m_rp; }

    void mapmem(traits::is_callable<inst *> auto fn) {
      voo::mapmem mm { m_inst.memory() };
      fn(static_cast<inst *>(*mm));
    }

    void update_atlas(vee::image_view::type iv, vee::sampler::type smp) {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, iv, smp);
    }

    void cmd_render_pass(vee::command_buffer cb, const voo::swapchain & sw) {
      upc pc { sw.aspect() };
      auto scb = voo::cmd_render_pass({
        .command_buffer = cb,
        .render_pass = *m_rp,
        .framebuffer = sw.framebuffer(),
        .extent = sw.extent(),
        .clear_colours {
          vee::clear_colour(0, 0, 0, 0),
          vee::clear_colour(0, 0, 0, 0),
        },
      });
      vee::cmd_set_viewport(*scb, sw.extent());
      vee::cmd_set_scissor(*scb, sw.extent());
      vee::cmd_push_vert_frag_constants(*scb, *m_pl, &pc);
      vee::cmd_bind_descriptor_set(*scb, *m_pl, 0, m_ds.descriptor_set());
      vee::cmd_bind_gr_pipeline(*scb, *m_ppl);
      vee::cmd_bind_vertex_buffers(*scb, 1, m_inst.buffer());
      m_quad.run(*scb, 0, 256);
    }
  };
}
