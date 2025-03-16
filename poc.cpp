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
      vlk->map_atlas([](auto * ptr) {
        ptr['#'] = 0x77777777;
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;
        ptr['E'] = 0xFF007777;
        ptr['S'] = 0xFF000077;
      });
      vlk->map_instances([&](vlk::inst * i) {
        *i++ = { .pos { 0, 0 }, .uv = vlk::id_to_uv('#') };
        *i++ = { .pos { 3, 3 }, .uv = vlk::id_to_uv('S') };
        return i;
      });

      extent_loop([&] {
        vlk->present();
      });
    });
  }
} i;
