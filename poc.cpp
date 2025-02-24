#pragma leco app
#pragma leco add_shader "poc.frag"
#pragma leco add_shader "poc.vert"

import casein;
import dotz;
import hai;
import jute;
import silog;
import vee;
import voo;
import vapp;
import wagen;

static constexpr const auto inst_count = 256;
struct inst_t {
  dotz::vec2 id;
};

static constexpr const jute::view map {
 "................"
 "................"
 "....XXX........."
 "..XXXXX........."
 "..XXX..........."
 "...XXXX........."
 "....X..........."
 "................"
 "................"
 "................"
 "................"
 "................"
 "................"
 "................"
 "................"
 "................"
};
static_assert(map.size() == 256);

static const class uv_map {
  dotz::vec2 m_data[256];
public:
  constexpr uv_map() {
    m_data['X'] = { 0, 0 };
    m_data['.'] = { 1, 0 };
  }
  auto operator[](unsigned idx) const { return m_data[idx]; };
} uvs {};

struct : vapp {
  void run() override {
    static constexpr const auto select_format = VK_FORMAT_R8G8B8A8_UNORM;

    main_loop("yorg", [&](auto & dq) {
      voo::one_quad quad { dq.physical_device() };
      voo::host_buffer inst {
        dq.physical_device(),
        vee::create_buffer(
          inst_count * sizeof(inst_t),
          vee::buffer_usage::vertex_buffer
        )
      };
      {
        voo::mapmem mm { inst.memory() };
        auto * ptr = static_cast<inst_t *>(*mm);
        for (auto i = 0; i < inst_count; i++) {
          ptr[i].id = uvs[map[i]];
        }
      }

      voo::h2l_image terrain {
        dq.physical_device(), 16, 16, VK_FORMAT_R8G8B8A8_SRGB
      };
      {
        voo::mapmem mm { terrain.host_memory() };
        auto * ptr = static_cast<unsigned *>(*mm);
        ptr[0] = 0xFF007700;
        ptr[1] = 0xFF770000;
      }
      voo::single_cb load_cb { dq.queue_family() };
      {
        voo::cmd_buf_one_time_submit ots { load_cb.cb() };
        terrain.setup_copy(load_cb.cb());
      }
      dq.queue()->queue_submit({ .command_buffer = load_cb.cb() });

      auto rp = vee::create_render_pass(vee::create_render_pass_params {
        .attachments {{
          vee::create_colour_attachment(dq.physical_device(), dq.surface()),
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
      });

      vee::sampler smp = vee::create_sampler(vee::nearest_sampler);

      voo::single_dset ds {
        vee::dsl_fragment_sampler(),
        vee::combined_image_sampler(),
      };
      vee::update_descriptor_set(ds.descriptor_set(), 0, terrain.iv(), *smp);

      struct upc { float aspect; };
      auto pl = vee::create_pipeline_layout(
        { ds.descriptor_set_layout() },
        { vee::vert_frag_push_constant_range<upc>() }
      );
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
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
          quad.vertex_input_bind(),
          vee::vertex_input_bind_per_instance(sizeof(inst_t)),
        },
        .attributes {
          quad.vertex_attribute(0),
          vee::vertex_attribute_vec2(1, 0),
        },
      });;

      voo::host_buffer hbuf { dq.physical_device(), vee::create_transfer_dst_buffer(16) };

      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };
      hai::array<voo::offscreen::colour_buffer> sel_buf { sw.count() };

      sw.create_framebuffers([&](auto i, auto iv) {
        sel_buf[i] = {
          dq.physical_device(), sw.extent(), select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };

        return vee::create_framebuffer({
          .physical_device = dq.physical_device(),
          .surface = dq.surface(),
          .render_pass = *rp,
          .attachments {{ iv, sel_buf[i].image_view() }},
        });
      });

      extent_loop([&] {
        voo::present_guard pg { dq.queue(), &sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { cb.cb() };
          {
            upc pc { sw.aspect() };
            auto scb = voo::cmd_render_pass({
              .command_buffer = *pcb,
              .render_pass = *rp,
              .framebuffer = sw.framebuffer(),
              .extent = sw.extent(),
              .clear_colours {
                vee::clear_colour(0, 0, 0, 0),
                vee::clear_colour(0, 0, 0, 0),
              },
            });
            vee::cmd_set_viewport(*scb, sw.extent());
            vee::cmd_set_scissor(*scb, sw.extent());
            vee::cmd_push_vert_frag_constants(*scb, *pl, &pc);
            vee::cmd_bind_descriptor_set(*scb, *pl, 0, ds.descriptor_set());
            vee::cmd_bind_gr_pipeline(*scb, *gp);
            vee::cmd_bind_vertex_buffers(*scb, 1, inst.buffer());
            quad.run(*scb, 0, 256);
          }

          int mx = casein::mouse_pos.x * casein::screen_scale_factor;
          int my = casein::mouse_pos.y * casein::screen_scale_factor;
          vee::cmd_copy_image_to_buffer(*pcb, { mx, my }, { 1, 1 }, sel_buf[sw.index()].image(), hbuf.buffer());
        }
        sync.queue_submit(dq.queue(), cb.cb());

        //voo::mapmem mm { hbuf.memory() };
        //auto * m = static_cast<unsigned char *>(*mm);
        //for (auto i = 0; i < 4; i++) silog::trace(m[i]);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
