#pragma leco app

import battlemap;
import casein;
import cursor;
import dotz;
import enemies;
import pick;
import selection;
import soldiers;
import spr;
import vee;
import voo;
import vapp;

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
      voo::single_cb cb { dq.queue_family() };
      voo::frame_sync_stuff sync {};
      voo::swapchain sw { dq };

      cursor::t cur { &dq, sw };

      pick::offscreen ofs { dq.physical_device(), &sw };

      battlemap::system map { dq, sw, ofs };
      soldiers::system sld { dq, sw, ofs };
      enemies::system ene { dq, sw, ofs };
      selection::system sel { dq, sw };
      pick::system pick { dq, ofs };
      update_pick(pick);

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
          ene.cmd_render_pass(cb.cb(), sw);
          sel.cmd_render_pass(cb.cb(), sw);
          if (mouse_in) pick.run(cb.cb(), ofs, mx, my);
          cur.run(cb.cb(), sw);
        }
        sync.queue_submit(dq.queue(), cb.cb());

        // XXX: Should this be inside the present guard?
        g_sel = mouse_in ? pick.pick() : -1;
        update_pick(pick);
        sel.set(g_sel_pos);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
