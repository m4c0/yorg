#pragma leco app

import battle;
import casein;
import cursor;
import dotz;
import render;
import selection;
import voo;
import vapp;

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
      render::system rnd { &dq };

      battle::system btl { &rnd };
      cursor::system cur { &rnd };
      selection::system sel { &rnd };

      extent_loop([&] {
        auto mouse = casein::mouse_pos;
        bool mouse_in =
          mouse.x >= 0 && mouse.x < casein::window_size.x &&
          mouse.y >= 0 && mouse.y < casein::window_size.y;
        int mx = mouse.x * casein::screen_scale_factor;
        int my = mouse.y * casein::screen_scale_factor;

        voo::present_guard pg { dq.queue(), &rnd.sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { cb.cb() };
          btl.cmd_render_pass(cb.cb(), rnd.sw);
          if (mouse_in) btl.run_pick(cb.cb(), rnd.ofs, mx, my);
          sel.cmd_render_pass(cb.cb(), rnd.sw);
          cur.run(cb.cb(), rnd.sw);
        }
        sync.queue_submit(dq.queue(), cb.cb());

        // XXX: Should this be inside the present guard?
        sel.set(mouse_in ? btl.pick() : -1);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
