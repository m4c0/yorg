export module battlemap;
import atlas;
import jute;
import pick;
import spr;
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
    "................"
    "................"
    "................"
    "................"
    "................"
    "................"
    "................"
    "................"
  };
  static_assert(map.size() == 256);

  static auto att(voo::device_and_queue & dq) {
    return vee::attachment_description {
      .format = dq.find_best_surface_image_format(),
      .load_op = vee::attachment_load_op_clear,
      .store_op = vee::attachment_store_op_store,
      .initial_layout = vee::image_layout_undefined,
      .final_layout = vee::image_layout_color_attachment_optimal,
    };
  }

  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    pick::system m_pick;
    atlas::t m_atlas;

    void load_sprites() {
      auto sm = m_spr.map();

      for (auto i = 0; i < 256; i++) {
        sm += {
          .pos { i % 16, i / 16 },
          .uv = atlas::id_to_uv(map[i]),
        };
      }
    }

  public:
    system(voo::device_and_queue & dq, const voo::swapchain & sw, const pick::offscreen & ofs)
      : m_spr { dq.physical_device(), sw, att(dq) }
      , m_pick { dq, ofs }
      , m_atlas { dq.physical_device(), dq.queue_family() }
    {
      m_atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr['X'] = 0xFF007700;
        ptr['.'] = 0xFF770000;
      });

      load_sprites();
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(vee::command_buffer cb, const voo::swapchain & sw) {
      m_spr.cmd_render_pass(cb, sw);
    }
  };
}
