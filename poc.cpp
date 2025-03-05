#pragma leco app

import atlas;
import casein;
import cursor;
import jute;
import pick;
import spr;
import vee;
import voo;
import vapp;

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
  enemy,
  selection,
};
static constexpr auto _(uv_ids e) { return static_cast<unsigned char>(e); }

static int g_sel = -1;

static void update_sprites(spr::system & spr, pick::system & pick) {
  auto pm = pick.map();
  auto sm = spr.map();

  const auto blit = [&](spr::inst i) { sm += i; };
  const auto blit_pick = [&](spr::inst i) {
    blit(i);

    pm += { .pos = i.pos };
    if (g_sel != pm.count() - 1) return;

    sm += {
      .pos = i.pos,
      .uv = atlas::id_to_uv(_(uv_ids::selection)),
    };
  };

  for (auto i = 0; i < 256; i++) {
    spr::inst ii {
      .pos { i % 16, i / 16 },
      .uv = atlas::id_to_uv(map[i]),
    };
    if (map[i] == '.') blit_pick(ii);
    else blit(ii);
  }
  blit_pick({
    .pos { 3, 1 },
    .uv = atlas::id_to_uv(_(uv_ids::soldier)),
  });
  blit({
    .pos { 7, 4 },
    .uv = atlas::id_to_uv(_(uv_ids::enemy)),
  });
}

struct init : vapp {
  init() {
    using namespace casein;

    handle(MOUSE_ENTER, [] {
      casein::cursor_visible = false;
      casein::interrupt(casein::IRQ_CURSOR);
    });
    handle(MOUSE_LEAVE, [] {
      casein::cursor_visible = true;
      casein::interrupt(casein::IRQ_CURSOR);
    });
  }
  void run() override {
    main_loop("yorg", [&](auto & dq) {
      atlas::t atlas { dq.physical_device(), dq.queue_family() };
      atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;

        ptr[_(uv_ids::soldier)] = 0xFF000077;
        ptr[_(uv_ids::enemy)] = 0xFF007777;
        ptr[_(uv_ids::selection)] = 0x77777777;
      });

      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };

      cursor::t cur { &dq, sw };

      spr::system spr { dq.physical_device(), dq.surface(), sw };
      pick::system pick { dq.physical_device(), dq.surface(), sw };
      update_sprites(spr, pick);

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
        g_sel = mouse_in ? pick.pick() : -1;
        update_sprites(spr, pick);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
