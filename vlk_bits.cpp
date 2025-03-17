module vlk;
import :clear;
import :cursor;
import :finish;
import :pick;
import :spr;
import vapp;

namespace vlk::impl {
  class bits : public vlk::bits {
    voo::single_cb m_cb { dq->queue_family() };
    voo::frame_sync_stuff m_sync {};

    clear m_clr {};
    spr m_spr {};
    cursor m_cur {};
    finish m_fin {};

    pick m_pck {};

  public:
    ~bits() { dq->queue()->device_wait_idle(); }

    void map_atlas(hai::fn<void, unsigned *> f) override { m_spr.map_atlas(f); }
    void map_instances(hai::fn<inst *, inst *> f) override { m_spr.map_instances(f); }
    void map_picks(hai::fn<pickable *, pickable *> f) override { m_pck.map(f); }
    void present() {
      voo::present_guard pg { dq->queue(), sw, &m_sync };
      {
        voo::cmd_buf_one_time_submit pcb { m_cb.cb() };
        m_clr.cmd_render_pass(*pcb);
        m_spr.cmd_render_pass(*pcb);
        m_cur.cmd_render_pass(*pcb);
        m_fin.cmd_render_pass(*pcb);

        m_pck.cmd_render_pass(*pcb);
      }
      m_sync.queue_submit(dq->queue(), m_cb.cb());
    }
    unsigned current_pick() override { return m_pck.current(); } 
  };
}

struct init : vapp {
  init() {
    using namespace casein;

    handle(MOUSE_ENTER, [] {
      casein::cursor_visible = false;
      casein::interrupt(casein::IRQ_CURSOR);
    });
    handle(MOUSE_LEAVE, [] {
      casein::cursor_visible = true;
      casein::interrupt(casein::IRQ_CURSOR);
    });
  }
  void run() override {
    main_loop("yorg", [&](auto & dq) {
      voo::swapchain sw { dq };

      vlk::dq = &dq;
      vlk::sw = &sw;

      vlk::impl::bits bits {};
      vlk::on_init(&bits);

      extent_loop([&] {
        bits.present();
        vlk::after_present(&bits);
      });
    });
  }
} i;
