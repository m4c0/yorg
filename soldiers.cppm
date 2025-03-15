export module soldiers;
import atlas;
import dotz;
import pick;
import render;
import spr;
import vee;
import voo;

namespace soldiers {
  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    pick::system m_pick;
    atlas::t m_atlas;

    void load_sprites() {
      auto sm = m_spr.map();

      sm += {
        .pos { 3, 1 },
        .uv = atlas::id_to_uv(1),
      };

      if (m_pick.pick()) {
        sm += {
          .pos = { 3, 1 },
          .uv = atlas::id_to_uv(2),
        };
      }
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
    dotz::vec2 pick() {
      load_sprites();
      return m_pick.pick() ? dotz::vec2 { 3, 1 } : -1;
    }

    void pickable(bool p) {
      auto pm = m_pick.map();
      if (!p) return;

      pm += {
        .pos { 3, 1 },
        .id = 1,
      };
    }
  };
}
