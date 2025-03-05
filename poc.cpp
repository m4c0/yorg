#pragma leco app

import atlas;
import battlemap;
import casein;
import cursor;
import dotz;
import jute;
import pick;
import soldiers;
import spr;
import vee;
import voo;
import vapp;

enum class uv_ids : unsigned char {
  soldier   = 1,
  enemy,
  selection,
};
static constexpr auto _(uv_ids e) { return static_cast<unsigned char>(e); }

static int g_sel = -1;
static dotz::vec2 g_sel_pos;

static void update_pick(pick::system & pick) {
  auto pm = pick.map();

  g_sel_pos = g_sel == -1 ? dotz::vec2 { 10000, 10000 } : pm[g_sel].pos;

  for (auto i = 0; i < 256; i++) {
    //if (map[i] != '.') continue;
    pm += { .pos { i % 16, i / 16 } };
  }
  pm += { .pos { 3, 1 } };
}

static void update_sprites(spr::system & spr) {
  auto sm = spr.map();

  sm += {
    .pos { 7, 4 },
    .uv = atlas::id_to_uv(_(uv_ids::enemy)),
  };

  sm += {
    .pos = g_sel_pos,
    .uv = atlas::id_to_uv(_(uv_ids::selection)),
  };
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
        ptr[_(uv_ids::soldier)] = 0xFF000077;
        ptr[_(uv_ids::enemy)] = 0xFF007777;
        ptr[_(uv_ids::selection)] = 0x77777777;
      });

      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };

      cursor::t cur { &dq, sw };

      battlemap::system map { dq, sw };
      soldiers::system sld { dq, sw };
      spr::system spr { dq.physical_device(), sw, {
        .format = dq.find_best_surface_image_format(),
        .load_op = vee::attachment_load_op_load,
        .store_op = vee::attachment_store_op_store,
        .initial_layout = vee::image_layout_color_attachment_optimal,
        .final_layout = vee::image_layout_color_attachment_optimal,
      }};
      pick::system pick { dq.physical_device(), dq.surface(), sw };
      update_pick(pick);
      update_sprites(spr);

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
          map.cmd_render_pass(cb.cb(), sw);
          sld.cmd_render_pass(cb.cb(), sw);
          spr.cmd_render_pass(cb.cb(), sw);
          if (mouse_in) pick.run(cb.cb(), sw, mx, my);
          cur.run(cb.cb(), sw);
        }
        sync.queue_submit(dq.queue(), cb.cb());

        // XXX: Should this be inside the present guard?
        g_sel = mouse_in ? pick.pick() : -1;
        update_pick(pick);
        update_sprites(spr);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
