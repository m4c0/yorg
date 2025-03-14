#pragma leco app

import battle;
import clearpass;
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
      voo::frame_sync_stuff sync {};
      render::system rnd { &dq };

      clearpass::system clr { &rnd };
      battle::system btl { &rnd };
      cursor::system cur { &rnd };
      selection::system sel { &rnd };

      extent_loop([&] {
        auto mouse = casein::mouse_pos;
        bool mouse_in =
          mouse.x >= 0 && mouse.x < casein::window_size.x &&
          mouse.y >= 0 && mouse.y < casein::window_size.y;

        voo::present_guard pg { dq.queue(), &rnd.sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { rnd.cb.cb() };
          clr.cmd_render_pass(&rnd);
          btl.cmd_render_pass(&rnd);
          sel.cmd_render_pass(&rnd);
          cur.cmd_render_pass(&rnd);

          btl.run_pick(&rnd);
        }
        sync.queue_submit(dq.queue(), rnd.cb.cb());

        // XXX: Should this be inside the present guard?
        sel.set(mouse_in ? btl.pick() : -1);
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
