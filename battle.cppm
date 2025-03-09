export module battle;
import battlemap;
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
    state m_state;

    battlemap::system m_map;
    soldiers::system m_sld;
    enemies::system m_ene;

  public:
    system(voo::device_and_queue & dq, voo::swapchain & sw, pick::offscreen & ofs)
      : m_map { dq, sw, ofs }
      , m_sld { dq, sw, ofs }
      , m_ene { dq, sw, ofs }
    {}

    void cmd_render_pass(vee::command_buffer cb, voo::swapchain & sw) {
      m_map.cmd_render_pass(cb, sw);
      m_sld.cmd_render_pass(cb, sw);
      m_ene.cmd_render_pass(cb, sw);
    }
    void run_pick(vee::command_buffer cb, pick::offscreen & ofs, int mx, int my) {
      switch (m_state) {
        case state::pick_soldier:
          m_sld.run_pick(cb, ofs, mx, my);
          break;
        case state::pick_target:
          m_map.run_pick(cb, ofs, mx, my);
          m_ene.run_pick(cb, ofs, mx, my);
          break;
      }
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
