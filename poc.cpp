#pragma leco app

import casein;
import dotz;
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

      extent_loop([&] {
        vlk->present();
      });
    });
  }
} i;
