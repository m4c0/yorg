#pragma leco app

import battle;
import clearpass;
import casein;
import cursor;
import dotz;
import render;
import selection;
import vlk;
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
      auto vlk = vlk::bits::create(&dq);

      voo::frame_sync_stuff sync {};
      render::system rnd { &dq };

      clearpass::system clr { &rnd };
      battle::system btl { &rnd };
      cursor::system cur { &rnd };
      selection::system sel { &rnd };

      extent_loop([&] {
        voo::present_guard pg { dq.queue(), &rnd.sw, &sync };
        {
          voo::cmd_buf_one_time_submit pcb { rnd.cb.cb() };
          clr.cmd_render_pass(&rnd);
          btl.cmd_render_pass(&rnd);
          sel.cmd_render_pass(&rnd);
          cur.cmd_render_pass(&rnd);
        }
        sync.queue_submit(dq.queue(), rnd.cb.cb());

        btl.pick();
        //sel.set(btl.pick());
      });
      dq.queue()->device_wait_idle();
    });
  }
} i;
