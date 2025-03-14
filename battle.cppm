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
    auto pick() {
      auto res = m_map.pick();
      if (res.x >= 0) return res;
      res = m_sld.pick();
      if (res.x >= 0) return res;
      res = m_ene.pick();
      return res;
    }
  };
}
