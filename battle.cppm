export module battle;
import battlemap;
import enemies;
import pick;
import render;
import soldiers;
import voo;

namespace battle {
  enum class state {
    pick_soldier,
    pick_target,
  };

  export class system {
    state m_state;

    battlemap::system m_map;
    soldiers::system m_sld;
    enemies::system m_ene;

  public:
    system(render::system * rnd)
      : m_map { rnd }
      , m_sld { rnd }
      , m_ene { rnd }
    {
      m_sld.pickable(true);
    }

    void cmd_render_pass(render::system * rnd) {
      m_map.cmd_render_pass(rnd);
      m_sld.cmd_render_pass(rnd);
      m_ene.cmd_render_pass(rnd);
    }
    void run_pick(render::system * rnd, int mx, int my) {
      m_sld.run_pick(rnd, mx, my);
      m_map.run_pick(rnd, mx, my);
      m_ene.run_pick(rnd, mx, my);
    }
    auto pick() {
      switch (m_state) {
        case state::pick_soldier:
          return m_sld.pick();
        case state::pick_target:
          auto res = m_map.pick();
          return (res.x < 0) ? m_ene.pick() : res;
      }
    }
  };
}
