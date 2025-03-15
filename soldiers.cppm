export module soldiers;
import atlas;
import dotz;
import pick;
import render;
import spr;
import vee;
import voo;

namespace soldiers {
  static constexpr const dotz::vec2 list[] {
    { 3,  1 },
    { 8, 12 },
  };

  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    pick::system m_pick;
    atlas::t m_atlas;

    dotz::vec2 load_sprites() {
      auto sm = m_spr.map();
      for (auto & e : list) {
        sm += {
          .pos = e,
          .uv = atlas::id_to_uv(1),
        };
      }

      auto p = m_pick.pick();
      if (p > 0) {
        sm += {
          .pos = list[p - 1],
          .uv = atlas::id_to_uv(2),
        };
        return list[p - 1];
      }

      return -1;
    }

  public:
    system(render::system * rnd)
      : m_spr { rnd }
      , m_pick { rnd->dq, rnd->ofs }
      , m_atlas { rnd }
    {
      m_atlas.mapmem([](auto * ptr) {
        ptr[1] = 0xFF000077;
        ptr[2] = 0x77777777;
      });

      load_sprites();
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(render::system * rnd) {
      m_spr.cmd_render_pass(rnd);
      m_pick.run(rnd);
    }
    dotz::vec2 pick() { return load_sprites(); }

    void pickable(bool p) {
      auto pm = m_pick.map();
      if (!p) return;

      unsigned i = 1;
      for (auto & e : list) {
        pm += {
          .pos = e,
          .id = i++,
        };
      }
    }
  };
}
