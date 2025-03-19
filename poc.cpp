#pragma leco app

import casein;
import dotz;
import state;
import vlk;

static int g_sel = -1;
static int g_soldier_sel = -1;

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
  if (g_soldier_sel >= 0) {
    int x = g_soldier_sel % 16;
    int y = g_soldier_sel / 16;
    *i++ = { .pos { x, y }, .uv = vlk::id_to_uv('#') };
  }
  return i;
}
static auto pick_soldier_target(vlk::pickable * i) {
  state::battlemap::foreach([&](unsigned x, unsigned y, char c) {
    if (c != '.') return;

    dotz::vec2 pos { x, y };
    dotz::vec2 sld_pos { g_soldier_sel % 16, g_soldier_sel / 16 };
    if (dotz::length(pos - sld_pos) > 4) return;

    *i++ = { .pos = pos, .id = y * 16 + x + 1 };
  });
  state::enemies::foreach([&](unsigned x, unsigned y) {
    dotz::vec2 pos { x, y };
    dotz::vec2 sld_pos { g_soldier_sel % 16, g_soldier_sel / 16 };
    if (dotz::length(pos - sld_pos) > 8) return;

    *i++ = { .pos = pos, .id = y * 16 + x + 1 };
  });
  return i;
}
static auto pick_soldier(vlk::pickable * i) {
  state::soldiers::foreach([&](unsigned x, unsigned y) {
    *i++ = { .pos { x, y }, .id = y * 16 + x + 1 };
  });
  return i;
}

static void mouse_down() {
  if (g_sel < 0) return;
  if (g_soldier_sel < 0) {
    g_soldier_sel = g_sel;
    vlk::map_picks(pick_soldier_target);
    return;
  }

  dotz::ivec2 sld_pos { g_soldier_sel % 16, g_soldier_sel / 16 };
  dotz::ivec2 sel_pos { g_sel % 16, g_sel / 16 };
  if (state::soldiers::has(sel_pos.x, sel_pos.y)) {
  } else if (state::enemies::has(sel_pos.x, sel_pos.y)) {
  } else {
    state::soldiers::at(sld_pos.x, sld_pos.y) = sel_pos;
    vlk::map_instances(instances);
  }

  g_soldier_sel = -1;
  g_sel = -1;
  vlk::map_picks(pick_soldier);
}

static auto & i = vlk::on_init = [] {
  vlk::map_atlas(atlas);
  vlk::map_instances(instances);
  vlk::map_picks(pick_soldier);

  casein::handle(casein::MOUSE_DOWN, mouse_down);
};
static auto & p = vlk::after_present = [] {
  auto s = vlk::current_pick() - 1;
  if (s != g_sel) {
    g_sel = s;
    vlk::map_instances(instances);
  }
};
