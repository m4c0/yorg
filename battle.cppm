export module battle;
import battlemap;
import casein;
import enemies;
import pick;
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

    void soldier_picked();
    void start_turn();

  public:
    system(render::system * rnd)
      : m_map { rnd }
      , m_sld { rnd }
      , m_ene { rnd }
    {
      start_turn();
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

module :private;

using namespace casein;

void battle::system::soldier_picked() {
  m_sld.pickable(false);
  m_map.pickable(true);
  m_ene.pickable(true);
  handle(MOUSE_UP, { this, &system::start_turn });
}
void battle::system::start_turn() {
  m_sld.pickable(true);
  m_map.pickable(false);
  m_ene.pickable(false);
  handle(MOUSE_UP, { this, &system::soldier_picked });
}

