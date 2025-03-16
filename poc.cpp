#pragma leco app

import battle;
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

      render::system rnd { &dq };

      battle::system btl { &rnd };
      cursor::system cur { &rnd };
      selection::system sel { &rnd };

      extent_loop([&] {
        vlk->present([&] {
          btl.cmd_render_pass(&rnd);
          sel.cmd_render_pass(&rnd);
          cur.cmd_render_pass(&rnd);
        });

        btl.pick();
        //sel.set(btl.pick());
      });
    });
  }
} i;
