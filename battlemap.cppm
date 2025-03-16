export module battlemap;
import dotz;
import jute;
import pick;
import vee;
import voo;

namespace battlemap {
  static constexpr const jute::view map {
    "................"
    "................"
    "....XXX........."
    "..XXXXX........."
    "..XXX..........."
    "...XXXX........."
    "....X..........."
    "................"
    "..........X....."
    "..........XXX..."
    "..........XXXXX."
    "............XXX."
    "..............X."
    "................"
    "................"
    "................"
  };
  static_assert(map.size() == 256);

  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    pick::system m_pick;
    atlas::t m_atlas;

    void load_sprites() {
      auto sm = m_spr.map();

      for (auto i = 0U; i < 256; i++) {
        sm += {
          .pos { i % 16, i / 16 },
          .uv = atlas::id_to_uv(map[i]),
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
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;
      });

      load_sprites();
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(render::system * rnd) {
      m_spr.cmd_render_pass(rnd);
      m_pick.run(rnd);
    }
    dotz::vec2 pick() {
      auto p = m_pick.pick();
      if (p == 0) return -1;
      return { (p - 1) % 16, (p - 1) / 16 };
    }

    void pickable(bool p) {
      auto pm = m_pick.map();
      if (!p) return;

      for (auto i = 0U; i < 256; i++) {
        if (map[i] == '.') pm += {
          .pos { i % 16, i / 16 },
          .id = i + 1,
        };
      }
    }
  };
}
