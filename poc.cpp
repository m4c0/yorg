#pragma leco app
#pragma leco add_shader "poc.frag"
#pragma leco add_shader "poc.vert"

import casein;
import dotz;
import vee;
import voo;
import vapp;

static constexpr const auto inst_count = 2;
struct inst_t {
  dotz::vec2 pos;
};

struct : vapp {
  void run() override {
    main_loop("yorg", [&](auto & dq, auto & sw) {
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
        ptr[0].pos = { 0, 0 };
        ptr[1].pos = { 1, 0 };
      }

      auto pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
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

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass({ *pcb });
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_vertex_buffers(*scb, 1, inst.buffer());
          quad.run(*scb, 0, 2);
        });
      });
    });
  }
} i;
