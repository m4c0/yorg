#pragma leco app

import casein;
import battlemap;
import dotz;
import enemies;
import soldiers;
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
        i = battlemap::load_sprites(i);
        i = enemies::load_sprites(i);
        i = soldiers::load_sprites(i);
        return i;
      });

      extent_loop([&] {
        vlk->present();
      });
    });
  }
} i;
