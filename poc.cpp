#pragma leco app

import battlemap;
import enemies;
import soldiers;
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
  battlemap::foreach([&](int x, int y, char c) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv(c) };
  });
  enemies::foreach([&](int x, int y) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('E') };
  });
  soldiers::foreach([&](int x, int y) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('S') };
  });

  if (g_sel >= 0) {
    int x = g_sel % 16;
    int y = g_sel / 16;
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('#') };
  }
  return i;
}
static auto picks(vlk::pickable * i) {
  battlemap::foreach([&](unsigned x, unsigned y, char c) {
    if (c != '.') return;
    *i++ = { .pos { x, y }, .id = y * 16 + x + 1 };
  });
  return i;
}

static auto & i = vlk::on_init = [](auto * vlk) {
  vlk->map_atlas(atlas);
  vlk->map_instances(instances);
  vlk->map_picks(picks);
};
static auto & p = vlk::after_present = [](auto * vlk) {
  auto s = vlk->current_pick() - 1;
  if (s != g_sel) {
    g_sel = s;
    vlk->map_instances(instances);
  }
};
