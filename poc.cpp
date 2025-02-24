#pragma leco app
#pragma leco add_shader "poc.frag"
#pragma leco add_shader "poc.vert"

import atlas;
import casein;
import dotz;
import hai;
import jute;
import silog;
import spr;
import vee;
import voo;
import vapp;
import wagen;

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

struct : vapp {
  void run() override {
    main_loop("yorg", [&](auto & dq) {
      spr::system spr { dq.physical_device(), dq.surface() };
      spr.mapmem(256, [](spr::inst * ptr) -> void {
        for (auto i = 0; i < 256; i++) {
          ptr[i] = {
            .pos { i % 16, i / 16 },
            .uv = atlas::id_to_uv(map[i]),
          };
        }
      });

      atlas::t atlas { dq.physical_device(), dq.queue_family() };
      atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;
      });

      vee::sampler smp = vee::create_sampler(vee::nearest_sampler);
      spr.update_atlas(atlas.image_view(), *smp);

      voo::host_buffer hbuf { dq.physical_device(), vee::create_transfer_dst_buffer(16) };

      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };
      hai::array<voo::offscreen::colour_buffer> sel_buf { sw.count() };

      sw.create_framebuffers([&](auto i, auto iv) {
        sel_buf[i] = {
          dq.physical_device(), sw.extent(), spr::select_format,
          vee::image_usage_colour_attachment,
          vee::image_usage_transfer_src
        };

        return vee::create_framebuffer({
          .physical_device = dq.physical_device(),
          .surface = dq.surface(),
          .render_pass = spr.render_pass(),
          .attachments {{ iv, sel_buf[i].image_view() }},
        });
      });

      extent_loop([&] {
        voo::present_guard pg { dq.queue(), &sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { cb.cb() };
          spr.cmd_render_pass(cb.cb(), sw);

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
