export module soldiers;
import atlas;
import spr;
import vee;
import voo;

namespace soldiers {
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

    void load_sprites() {
      auto sm = m_spr.map();
      sm += {
        .pos { 3, 1 },
        .uv = atlas::id_to_uv(1),
      };
    }

  public:
    system(voo::device_and_queue & dq, const voo::swapchain & sw)
      : m_spr { dq.physical_device(), sw, att(dq) }
      , m_atlas { dq.physical_device(), dq.queue_family() }
    {
      m_atlas.mapmem(dq.queue(), [](auto * ptr) {
        ptr[1] = 0xFF000077;
      });

      load_sprites();
      m_spr.update_atlas(m_atlas.image_view(), *m_smp);
    }

    void cmd_render_pass(vee::command_buffer cb, const voo::swapchain & sw) {
      m_spr.cmd_render_pass(cb, sw);
    }
  };
}
