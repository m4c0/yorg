#pragma leco app

import casein;
import state;
import vlk;

static int g_sel = -1;

static auto atlas(auto * ptr) {
  ptr['#'] = 0x77999999;
  ptr['.'] = 0xFF003300;
  ptr['X'] = 0xFF770000;
  ptr['E'] = 0xFF003399;
  ptr['S'] = 0xFF000099;
}
static auto instances(vlk::inst * i) {
  state::battlemap::foreach([&](int x, int y, char c) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv(c) };
  });
  state::enemies::foreach([&](int x, int y) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('E') };
  });
  state::soldiers::foreach([&](int x, int y) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('S') };
  });

  if (g_sel >= 0) {
    int x = g_sel % 16;
    int y = g_sel / 16;
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('#') };
  }
  return i;
}
static auto pick_battle(vlk::pickable * i) {
  state::battlemap::foreach([&](unsigned x, unsigned y, char c) {
    if (c != '.') return;
    *i++ = { .pos { x, y }, .id = y * 16 + x + 1 };
  });
  return i;
}
static auto pick_soldier(vlk::pickable * i) {
  state::soldiers::foreach([&](unsigned x, unsigned y) {
    *i++ = { .pos { x, y }, .id = y * 16 + x + 1 };
  });
  return i;
}

static auto & i = vlk::on_init = [] {
  vlk::map_atlas(atlas);
  vlk::map_instances(instances);
  vlk::map_picks(pick_soldier);
};
static auto & p = vlk::after_present = [] {
  auto s = vlk::current_pick() - 1;
  if (s != g_sel) {
    g_sel = s;
    vlk::map_instances(instances);
  }
};

static auto zz = [] {
  using namespace casein;
  handle(MOUSE_DOWN, [] {
    if (g_sel != -1) {
      vlk::map_picks(pick_battle);
    }
  });
  return 0;
}();
