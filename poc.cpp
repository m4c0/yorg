#pragma leco app

import casein;
import dotz;
import rng;
import state;
import vlk;

static constexpr const auto max_move = 4;

static int g_sel = -1;
static int g_soldier_sel = -1;

static void enemy_turn();

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
  for (auto pos: state::enemies::list()) {
    *i++ = { .pos = pos, .uv = vlk::id_to_uv('E') };
  }
  for (auto pos: state::soldiers::list()) {
    *i++ = { .pos = pos, .uv = vlk::id_to_uv('S') };
  }

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
    if (dotz::length(pos - sld_pos) > max_move) return;

    *i++ = { .pos = pos, .id = y * 16 + x + 1 };
  });
  for (auto pos : state::enemies::list()) {
    dotz::vec2 sld_pos { g_soldier_sel % 16, g_soldier_sel / 16 };
    if (dotz::length(pos - sld_pos) > max_move) continue;

    unsigned id = pos.y * 16 + pos.x + 1;
    *i++ = { .pos = pos, .id = id };
  }
  return i;
}
static auto pick_soldier(vlk::pickable * i) {
  for (auto pos: state::soldiers::list()) {
    unsigned id = pos.y * 16 + pos.x + 1;
    *i++ = { .pos = pos, .id = id };
  }
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
    g_soldier_sel = g_sel;
    vlk::map_picks(pick_soldier_target);
    return;
  }

  if (state::enemies::has(sel_pos.x, sel_pos.y)) {
    state::enemies::hit(sel_pos.x, sel_pos.y);
  } else {
    state::soldiers::at(sld_pos.x, sld_pos.y) = sel_pos;
  }
  g_soldier_sel = -1;
  g_sel = -1;
  vlk::map_picks([](auto i) { return i; });
  vlk::map_instances(instances);
  enemy_turn();
}

static void enemy_turn() {
  const auto & l = state::enemies::list();
  if (l.size() == 0) return;

  auto & e = l[rng::rand(l.size())];
  dotz::ivec2 s { 10000 };
  for (auto & pos : state::soldiers::list()) {
    auto dist = dotz::length(e - pos);
    if (dist > max_move) continue;
    if (dist > dotz::length(s - e)) continue;
    s = pos;
  }
  if (state::soldiers::has(s.x, s.y)) {
  } else {
  }

  vlk::map_picks(pick_soldier);
  vlk::map_instances(instances);
}

static auto & i = vlk::on_init = [] {
  state::enemies::init();
  state::soldiers::init();

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
