#pragma leco app

import atlas;
import casein;
import cursor;
import dotz;
import hai;
import jute;
import pick;
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

enum class uv_ids : unsigned char {
  soldier   = 1,
  selection,
};
static constexpr auto _(uv_ids e) { return static_cast<unsigned char>(e); }

static int g_sel = -1;

static void update_picks(pick::system & pick) {
  pick.mapmem([](pick::inst * ptr) -> pick::inst * {
    for (auto i = 0; i < 256; i++) {
      *ptr++ = { .pos { i % 16, i / 16 } };
    }
    return ptr;
  });
}
static void update_sprites(spr::system & spr) {
  spr.mapmem([](spr::inst * ptr) -> spr::inst * {
    for (auto i = 0; i < 256; i++) {
      *ptr++ = {
        .pos { i % 16, i / 16 },
        .uv = atlas::id_to_uv(map[i]),
      };
    }
    *ptr++ = {
      .pos { 3, 1 },
      .uv = atlas::id_to_uv(_(uv_ids::soldier)),
    };
    if (g_sel == -1) return ptr;

    *ptr++ = {
      .pos { g_sel % 16, g_sel / 16 },
      .uv = atlas::id_to_uv(_(uv_ids::selection)),
    };
    return ptr;
  });
}

struct : vapp {
  void run() override {
    casein::cursor_visible = false;
    casein::interrupt(casein::IRQ_CURSOR);

    main_loop("yorg", [&](auto & dq) {
      atlas::t atlas { dq.physical_device(), dq.queue_family() };
      atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;

        ptr[_(uv_ids::soldier)] = 0xFF000077;
        ptr[_(uv_ids::selection)] = 0x77777777;
      });

      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };

      cursor::t cur { &dq, sw };

      spr::system spr { dq.physical_device(), dq.surface(), sw };
      update_sprites(spr);

      pick::system pick { dq.physical_device(), dq.surface(), sw };
      update_picks(pick);

      vee::sampler smp = vee::create_sampler(vee::nearest_sampler);
      spr.update_atlas(atlas.image_view(), *smp);

      extent_loop([&] {
        auto mouse = casein::mouse_pos;
        bool mouse_in =
          mouse.x >= 0 && mouse.x < casein::window_size.x &&
          mouse.y >= 0 && mouse.y < casein::window_size.y;
        int mx = mouse.x * casein::screen_scale_factor;
        int my = mouse.y * casein::screen_scale_factor;

        voo::present_guard pg { dq.queue(), &sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { cb.cb() };
          spr.cmd_render_pass(cb.cb(), sw);
          if (mouse_in) pick.run(cb.cb(), sw, mx, my);
          cur.run(cb.cb(), sw);
        }
        sync.queue_submit(dq.queue(), cb.cb());

        // XXX: Should this be inside the present guard?
        if (mouse_in) {
          auto n = pick.pick();
          if (n >= 0) g_sel = n;
        } else g_sel = -1;
        update_sprites(spr);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
