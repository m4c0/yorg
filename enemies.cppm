export module enemies;
import atlas;
import dotz;
import pick;
import render;
import spr;
import vee;
import voo;

namespace enemies {
  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    pick::system m_pick;
    atlas::t m_atlas;

    void load_sprites() {
      auto sm = m_spr.map();
      auto pm = m_pick.map();

      sm += {
        .pos { 7, 4 },
        .uv = atlas::id_to_uv(1),
      };
      pm += {
        .pos { 7, 4 },
        .id = 1,
      };
    }

  public:
    system(render::system * rnd)
      : m_spr { rnd }
      , m_pick { rnd->dq, rnd->ofs }
      , m_atlas { rnd }
    {
      m_atlas.mapmem([](auto * ptr) {
        ptr[1] = 0xFF007777;
      });

      load_sprites();
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(render::system * rnd) { m_spr.cmd_render_pass(rnd); }
    void run_pick(vee::command_buffer cb, const render::offscreen & ofs, int mx, int my) {
      m_pick.run(cb, ofs, mx, my);
    }
    dotz::vec2 pick() {
      return m_pick.pick() ? dotz::vec2 { 3, 1 } : -1;
    }
  };
}
