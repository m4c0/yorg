export module selection;
import atlas;
import dotz;
import spr;
import vee;
import voo;

namespace selection {
  static auto att(voo::device_and_queue & dq) {
    return vee::attachment_description {
      .format = dq.find_best_surface_image_format(),
      .load_op = vee::attachment_load_op_load,
      .store_op = vee::attachment_store_op_store,
      .initial_layout = vee::image_layout_color_attachment_optimal,
      .final_layout = vee::image_layout_color_attachment_optimal,
    };
  }

  export class system {
    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    spr::system m_spr;
    atlas::t m_atlas;

    void load_sprites(dotz::vec2 pos) {
      auto sm = m_spr.map();
      sm += {
        .pos = pos,
        .uv = atlas::id_to_uv(1),
      };
    }

  public:
    system(voo::device_and_queue & dq, const voo::swapchain & sw)
      : m_spr { dq.physical_device(), sw, att(dq) }
      , m_atlas { dq.physical_device(), dq.queue_family() }
    {
      m_atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr[1] = 0x77777777;
      });

      load_sprites({ 10000, 10000 });
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(vee::command_buffer cb, const voo::swapchain & sw) {
      m_spr.cmd_render_pass(cb, sw);
    }

    void set(dotz::vec2 p) { load_sprites(p); }
  };
}
