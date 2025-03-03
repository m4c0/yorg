export module blit;
import atlas;
import dotz;
import pick;
import spr;
import voo;

namespace blit {
  export struct inst {
    dotz::vec2 pos;
    char id;
  };
  export constexpr const auto sel = 0x7f;
  export class system {
    voo::memiter<spr::inst> m_spr;
    voo::memiter<pick::inst> m_pick;
    int m_sel;

  public:
    system(spr::system & s, pick::system & p, int sel)
      : m_spr { s.map() }
      , m_pick { p.map() }
      , m_sel { sel }
    {}

    void blit(inst i) {
      m_spr += {
        .pos = i.pos,
        .uv = atlas::id_to_uv(i.id),
      };
    }
    [[nodiscard]] bool pick(inst i) {
      blit(i);

      bool res = m_sel == m_pick.count();
      m_pick += { .pos = i.pos };

      if (res) blit({ .pos = i.pos, .id = sel });
      
      return res;
    }
  };
}
