#pragma leco app

import battlemap;
import enemies;
import soldiers;
import vlk;

static int g_sel = -1;

static auto atlas(auto * ptr) {
  ptr['#'] = 0x77777777;
  ptr['X'] = 0xFF007700;
  ptr['.'] = 0xFF770000;
  ptr['E'] = 0xFF007777;
  ptr['S'] = 0xFF000077;
}
static auto instances(vlk::inst * i) {
  battlemap::foreach([&](int x, int y, char c) {
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv(c) };
  });

  i = enemies::load_sprites(i);
  i = soldiers::load_sprites(i);
  if (g_sel >= 0) {
    int x = g_sel % 16;
    int y = g_sel / 16;
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('#') };
  }
  return i;
}
static auto picks(vlk::pickable * i) {
  battlemap::foreach([&](unsigned x, unsigned y, char c) {
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
  auto s = vlk->current_pick();
  if (s != g_sel) {
    g_sel = s - 1;
    vlk->map_instances(instances);
  }
};
